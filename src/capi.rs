// Copyright 2022 Sutou Kouhei <kou@clear-code.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use std::boxed::Box;
use std::ffi::CStr;
use std::ffi::CString;
use std::future::Future;
use std::sync::Arc;

use datafusion::arrow::array::Array;
use datafusion::arrow::array::StructArray;
use datafusion::arrow::datatypes::Schema;
use datafusion::arrow::error::ArrowError;
use datafusion::arrow::ffi::ArrowArray;
use datafusion::arrow::ffi::ArrowArrayRef;
use datafusion::arrow::ffi::FFI_ArrowArray;
use datafusion::arrow::ffi::FFI_ArrowSchema;
use datafusion::arrow::record_batch::RecordBatch;
use datafusion::common::DataFusionError;
use datafusion::dataframe::DataFrame;
use datafusion::datasource::MemTable;
use datafusion::execution::context::SessionContext;
use datafusion::execution::options::CsvReadOptions;
use datafusion::execution::options::ParquetReadOptions;

fn strdup(rs_str: &str) -> *mut libc::c_char {
    unsafe {
        let c_str =
            libc::malloc(std::mem::size_of::<*mut libc::c_char>() * rs_str.len() + 1)
                as *mut libc::c_char;
        std::ptr::copy_nonoverlapping(rs_str.as_ptr() as *const i8, c_str, rs_str.len());
        let nul = "\0";
        std::ptr::copy_nonoverlapping(
            nul.as_ptr() as *const i8,
            c_str.add(rs_str.len()),
            nul.len(),
        );
        c_str
    }
}

fn c_string_to_str<'a>(
    c_string: *const libc::c_char,
) -> Result<&'a str, std::str::Utf8Error> {
    let cstr = unsafe { CStr::from_ptr(c_string) };
    cstr.to_str()
}

fn c_strings_to_strings(
    c_strings: *const *const libc::c_char,
    n: usize,
) -> Result<Vec<String>, std::ffi::IntoStringError> {
    let slice = unsafe { std::slice::from_raw_parts(c_strings, n) };
    let mut strings = Vec::new();
    for &c_string in slice {
        let cstring = CString::from(unsafe { CStr::from_ptr(c_string) });
        strings.push(cstring.into_string()?);
    }
    Ok(strings)
}

fn strings_to_c_strings(
    strings: &Vec<String>,
    n_c_strings: *mut usize,
) -> *mut *mut libc::c_char {
    let n = strings.len();
    unsafe {
        *n_c_strings = n;
    };
    let c_strings = unsafe { libc::malloc(std::mem::size_of::<*mut libc::c_char>() * n) }
        as *mut *mut libc::c_char;
    let slice = unsafe { std::slice::from_raw_parts_mut(c_strings, n) };
    for (i, string) in strings.iter().enumerate() {
        slice[i] = strdup(string);
    }
    c_strings
}

/// \enum DFErrorCode
/// \brief Error category
///
/// cbindgen:prefix-with-name
/// cbindgen:rename-all=ScreamingSnakeCase
#[repr(C)]
#[derive(Copy, Clone)]
#[allow(clippy::upper_case_acronyms)]
pub enum DFErrorCode {
    Arrow,
    Parquet,
    #[allow(dead_code)]
    Avro,
    ObjectStore,
    IO,
    SQL,
    NotImplemented,
    Internal,
    Plan,
    Schema,
    Execution,
    ResourcesExhausted,
    External,
    #[allow(dead_code)]
    JIT,
}

/// \struct DFError
/// \brief A struct that holds error information.
///
/// You can access to error information by `df_error_get_code()` and
/// `df_error_get_message()`.
///
/// You need to free error information by `df_error_free()` when no
/// longer needed.
pub struct DFError {
    code: DFErrorCode,
    message: Box<CStr>,
}

impl DFError {
    pub fn new(code: DFErrorCode, message: Box<CStr>) -> Self {
        Self { code, message }
    }
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_error_new(
    code: DFErrorCode,
    message: *const libc::c_char,
) -> Box<DFError> {
    let c_str_message = unsafe { CStr::from_ptr(message) };
    Box::new(DFError::new(code, Box::<CStr>::from(c_str_message)))
}

/// \brief Free the given `DFError`.
///
/// \param _error A `DFError` returned by `df_*()` functions.
///
/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called for the same `error` multiple
/// times.
#[no_mangle]
pub extern "C" fn df_error_free(_error: Option<Box<DFError>>) {}

/// \brief Get a message of this error.
///
/// \param error A `DFError`.
/// \return A message of this error.
///
/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called with `error` that is freed by
/// `df_error_free()`.
#[no_mangle]
pub extern "C" fn df_error_get_message(error: &mut DFError) -> *const libc::c_char {
    error.message.as_ptr()
}

/// \brief Get a code of this error.
///
/// \param error A `DFError`.
/// \return A code of this error.
///
/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called with `error` that is freed by
/// `df_error_free()`.
#[no_mangle]
pub extern "C" fn df_error_get_code(error: &mut DFError) -> DFErrorCode {
    error.code
}

trait IntoDFError {
    type Value;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value>;
}

fn df_error_set(error: *mut *mut DFError, code: DFErrorCode, message: &str) {
    if error.is_null() {
        return;
    }
    let c_string_message = match CString::new(message) {
        Ok(c_string_message) => c_string_message,
        Err(_) => return,
    };
    unsafe {
        *error = Box::into_raw(Box::new(DFError::new(
            code,
            c_string_message.into_boxed_c_str(),
        )));
    };
}

impl<V> IntoDFError for Result<V, ArrowError> {
    type Value = V;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value> {
        match self {
            Ok(value) => Some(value),
            Err(e) => {
                df_error_set(error, DFErrorCode::Arrow, &e.to_string());
                error_value
            }
        }
    }
}

impl<V> IntoDFError for Result<V, DataFusionError> {
    type Value = V;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value> {
        match self {
            Ok(value) => Some(value),
            Err(e) => {
                let code = match e {
                    DataFusionError::ArrowError(_) => DFErrorCode::Arrow,
                    DataFusionError::ParquetError(_) => DFErrorCode::Parquet,
                    #[cfg(feature = "avro")]
                    DataFusionError::AvroError(_) => DFErrorCode::Avro,
                    DataFusionError::ObjectStore(_) => DFErrorCode::ObjectStore,
                    DataFusionError::IoError(_) => DFErrorCode::IO,
                    DataFusionError::SQL(_) => DFErrorCode::SQL,
                    DataFusionError::NotImplemented(_) => DFErrorCode::NotImplemented,
                    DataFusionError::Internal(_) => DFErrorCode::Internal,
                    DataFusionError::Plan(_) => DFErrorCode::Plan,
                    DataFusionError::SchemaError(_) => DFErrorCode::Schema,
                    DataFusionError::Execution(_) => DFErrorCode::Execution,
                    DataFusionError::ResourcesExhausted(_) => {
                        DFErrorCode::ResourcesExhausted
                    }
                    DataFusionError::External(_) => DFErrorCode::External,
                    #[cfg(feature = "jit")]
                    DataFusionError::JITError(_) => DFErrorCode::JIT,
                };
                df_error_set(error, code, &e.to_string());
                error_value
            }
        }
    }
}

impl<V> IntoDFError for Result<V, std::str::Utf8Error> {
    type Value = V;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value> {
        match self {
            Ok(value) => Some(value),
            Err(e) => {
                df_error_set(error, DFErrorCode::External, &e.to_string());
                error_value
            }
        }
    }
}

impl<V> IntoDFError for Result<V, std::ffi::IntoStringError> {
    type Value = V;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value> {
        match self {
            Ok(value) => Some(value),
            Err(e) => {
                df_error_set(error, DFErrorCode::External, &e.to_string());
                error_value
            }
        }
    }
}

/// \struct DFArrowSchema
/// \brief Same as the `ArrowSchema` struct in the Arrow C data interface
///
/// See also: https://arrow.apache.org/docs/format/CDataInterface.html#the-arrowschema-structure
#[repr(C)]
#[derive(Debug)]
pub struct DFArrowSchema {
    format: *const libc::c_char,
    name: *const libc::c_char,
    metadata: *const libc::c_char,
    flags: i64,
    n_children: i64,
    children: *mut *mut DFArrowSchema,
    dictionary: *mut DFArrowSchema,
    release: Option<unsafe extern "C" fn(schema: *mut DFArrowSchema)>,
    private_data: *mut libc::c_void,
}

impl From<FFI_ArrowSchema> for Box<DFArrowSchema> {
    fn from(rs_ffi_schema: FFI_ArrowSchema) -> Self {
        let rs_ffi_schema_ptr = Box::into_raw(Box::new(rs_ffi_schema));
        unsafe { Box::from_raw(rs_ffi_schema_ptr as *mut DFArrowSchema) }
    }
}

/// \struct DFArrowArray
/// \brief Same as the `ArrowArray` struct in the Arrow C data interface
///
/// See also: https://arrow.apache.org/docs/format/CDataInterface.html#the-arrowarray-structure
#[repr(C)]
#[derive(Debug)]
pub struct DFArrowArray {
    length: i64,
    null_count: i64,
    offset: i64,
    n_buffers: i64,
    n_children: i64,
    buffers: *mut *const libc::c_void,
    children: *mut *mut DFArrowArray,
    dictionary: *mut DFArrowArray,
    release: Option<unsafe extern "C" fn(array: *mut DFArrowArray)>,
    private_data: *mut libc::c_void,
}

fn block_on<F: Future>(future: F) -> F::Output {
    tokio::runtime::Runtime::new().unwrap().block_on(future)
}

/// \struct DFDataFrame
/// \brief A struct for data frame.
///
/// You get execution result as a data frame.
///
/// You need to free data frame by `df_data_frame_free()` when no
/// longer needed.
pub struct DFDataFrame {
    data_frame: Arc<DataFrame>,
}

impl DFDataFrame {
    pub fn new(data_frame: Arc<DataFrame>) -> Self {
        Self { data_frame }
    }
}

/// \brief Free the given `DFDataFrame`.
///
/// \param _data_frame A `DFDataFrame`.
///
/// # Safety
///
/// This function should not be called for the same `data_frame`
/// multiple times.
#[no_mangle]
pub extern "C" fn df_data_frame_free(_data_frame: Option<Box<DFDataFrame>>) {}

/// \brief Show the given data frame contents to the standard output.
///
/// \param data_frame A `DFDataFrame` to be shown.
/// \param error Return location for a `DFError` or `NULL`.
#[no_mangle]
pub extern "C" fn df_data_frame_show(
    data_frame: &mut DFDataFrame,
    error: *mut *mut DFError,
) {
    let future = data_frame.data_frame.show();
    block_on(future).into_df_error(error, None);
}

#[no_mangle]
pub extern "C" fn df_data_frame_export(
    data_frame: &mut DFDataFrame,
    c_abi_schema_out: *mut *mut DFArrowSchema,
    c_abi_record_batches_out: *mut *mut *mut DFArrowArray,
    error: *mut *mut DFError,
) -> i64 {
    let option = || -> Option<i64> {
        let future = data_frame.data_frame.collect();
        let mut rs_record_batches = block_on(future).into_df_error(error, None)?;
        let n = rs_record_batches.len();
        let c_abi_record_batches = unsafe {
            libc::malloc(std::mem::size_of::<*mut DFArrowArray>() * n)
                as *mut *mut DFArrowArray
        };
        let c_abi_record_batch_slice =
            unsafe { std::slice::from_raw_parts_mut(c_abi_record_batches, n) };
        #[allow(clippy::needless_range_loop)]
        for i in 0..n {
            let rs_record_batch = rs_record_batches.remove(0);
            let rs_struct_array = StructArray::from(rs_record_batch);
            let (c_abi_record_batch, c_abi_schema) =
                rs_struct_array.to_raw().into_df_error(error, None)?;
            c_abi_record_batch_slice[i] = c_abi_record_batch as *mut DFArrowArray;
            if i == 0 {
                unsafe {
                    *c_abi_schema_out = c_abi_schema as *mut DFArrowSchema;
                }
            } else {
                unsafe { Arc::from_raw(c_abi_schema) };
            }
        }
        unsafe {
            *c_abi_record_batches_out = c_abi_record_batches;
        }
        Some(n as i64)
    }();
    match option {
        Some(n) => n,
        None => {
            unsafe {
                if !(*c_abi_record_batches_out).is_null() {
                    libc::free((*c_abi_record_batches_out) as *mut libc::c_void);
                    *c_abi_record_batches_out = std::ptr::null_mut();
                }
            }
            -1
        }
    }
}

/// \struct DFSessionContext
/// \brief An entry point of DataFusion API.
///
/// You need to create `DFSessionContext` to use DataFusion API.
pub struct DFSessionContext {
    context: SessionContext,
}

impl DFSessionContext {
    pub fn new(context: SessionContext) -> Self {
        Self { context }
    }
}

/// \brief Create a new `DFSessionContext`.
///
/// \return A newly created `DFSessionContext`.
///
///   It should be freed by `df_session_context_free()` when no longer
///   needed.
#[no_mangle]
pub extern "C" fn df_session_context_new() -> Box<DFSessionContext> {
    Box::new(DFSessionContext::new(SessionContext::new()))
}

/// \brief Free the given `DFSessionContext`.
///
/// \param _context A `DFSessionContext` created by
///   `df_session_context_new()`.
///
/// # Safety
///
/// This function should not be called with `context` that is not
/// created by `df_session_context_new()`.
///
/// This function should not be called for the same `context`
/// multiple times.
#[no_mangle]
pub extern "C" fn df_session_context_free(_context: Option<Box<DFSessionContext>>) {}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_sql(
    context: &mut DFSessionContext,
    sql: *const libc::c_char,
    error: *mut *mut DFError,
) -> Option<Box<DFDataFrame>> {
    let cstr_sql = unsafe { CStr::from_ptr(sql) };
    let maybe_rs_sql = cstr_sql.to_str().into_df_error(error, None);
    let rs_sql = match maybe_rs_sql {
        Some(rs_sql) => rs_sql,
        None => return None,
    };
    let result = block_on(context.context.sql(rs_sql));
    let maybe_data_frame = result.into_df_error(error, None);
    maybe_data_frame.map(|data_frame| Box::new(DFDataFrame::new(data_frame)))
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_deregister(
    context: &mut DFSessionContext,
    name: *const libc::c_char,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        let rs_name = c_string_to_str(name).into_df_error(error, None)?;
        context
            .context
            .deregister_table(rs_name)
            .into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_register_record_batches(
    context: &mut DFSessionContext,
    name: *const libc::c_char,
    c_abi_schema: &mut DFArrowSchema,
    c_abi_record_batches: &mut *mut DFArrowArray,
    n_record_batches: libc::size_t,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        let cstr_name = unsafe { CStr::from_ptr(name) };
        let rs_name = cstr_name.to_str().into_df_error(error, None)?;
        let mut rs_ffi_schema =
            (c_abi_schema as *mut DFArrowSchema) as *mut FFI_ArrowSchema;
        let mut rs_record_batches = Vec::new();
        let c_abi_record_batch_slice =
            unsafe { std::slice::from_raw_parts(c_abi_record_batches, n_record_batches) };
        for c_abi_record_batch in c_abi_record_batch_slice {
            let rs_ffi_record_batch =
                (*c_abi_record_batch as *mut DFArrowArray) as *mut FFI_ArrowArray;
            // We want to share rs_ffi_schema by passing
            // Arc<FFI_ArrowSchema> but arrow-rs's ArrowArray API
            // doesn't accept it...
            let rs_record_batch_array =
                unsafe { ArrowArray::try_from_raw(rs_ffi_record_batch, rs_ffi_schema) }
                    .into_df_error(error, None)?;
            let rs_record_batch_data =
                rs_record_batch_array.to_data().into_df_error(error, None)?;
            let rs_record_batch =
                RecordBatch::from(&StructArray::from(rs_record_batch_data));
            rs_record_batches.push(rs_record_batch);
            // ... So we export schema again here.
            rs_ffi_schema =
                ArrowArray::into_raw(rs_record_batch_array).1 as *mut FFI_ArrowSchema;
        }
        let rs_schema = Arc::new(
            Schema::try_from(unsafe { &*rs_ffi_schema }).into_df_error(error, None)?,
        );
        let rs_table = Arc::new(
            MemTable::try_new(rs_schema, vec![rs_record_batches])
                .into_df_error(error, None)?,
        );
        context
            .context
            .register_table(rs_name, rs_table)
            .into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

pub struct DFCSVReadOptions<'a> {
    options: CsvReadOptions<'a>,
    schema: Option<Schema>,
}

impl<'a> DFCSVReadOptions<'a> {
    pub fn new() -> Self {
        let options = CsvReadOptions::default();
        let schema = None;
        Self { options, schema }
    }
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_new<'a>() -> Box<DFCSVReadOptions<'a>> {
    Box::new(DFCSVReadOptions::new())
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_free(_options: Option<Box<DFCSVReadOptions>>) {}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_has_header(
    options: &mut DFCSVReadOptions,
    has_header: bool,
) {
    options.options.has_header = has_header;
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_has_header(
    options: &mut DFCSVReadOptions,
) -> bool {
    options.options.has_header
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_delimiter(
    options: &mut DFCSVReadOptions,
    delimiter: u8,
) {
    options.options.delimiter = delimiter;
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_delimiter(
    options: &mut DFCSVReadOptions,
) -> u8 {
    options.options.delimiter
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_schema<'a>(
    options: &'a mut DFCSVReadOptions<'a>,
    schema: Option<Box<DFArrowSchema>>,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        match schema {
            Some(mut s) => {
                let rs_ffi_schema =
                    (s.as_mut() as *mut DFArrowSchema) as *mut FFI_ArrowSchema;
                let rs_schema = Schema::try_from(unsafe { &*rs_ffi_schema })
                    .into_df_error(error, None)?;
                options.schema = Some(rs_schema);
                options.options.schema = options.schema.as_ref();
            }
            None => {
                options.schema = None;
                options.options.schema = None;
            }
        };
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_schema(
    options: &mut DFCSVReadOptions,
    error: *mut *mut DFError,
) -> Option<Box<DFArrowSchema>> {
    match options.options.schema {
        Some(rs_schema) => {
            let rs_ffi_schema =
                FFI_ArrowSchema::try_from(rs_schema).into_df_error(error, None)?;
            Some(Box::<DFArrowSchema>::from(rs_ffi_schema))
        }
        None => None,
    }
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_schema_infer_max_records(
    options: &mut DFCSVReadOptions,
    n: usize,
) {
    options.options.schema_infer_max_records = n;
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_schema_infer_max_records(
    options: &mut DFCSVReadOptions,
) -> usize {
    options.options.schema_infer_max_records
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_file_extension(
    options: &mut DFCSVReadOptions,
    file_extension: *const libc::c_char,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        options.options.file_extension =
            c_string_to_str(file_extension).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_file_extension(
    options: &mut DFCSVReadOptions,
) -> *mut libc::c_char {
    strdup(options.options.file_extension)
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_set_table_partition_columns(
    options: &mut DFCSVReadOptions,
    columns: *const *const libc::c_char,
    n_columns: usize,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        options.options.table_partition_cols =
            c_strings_to_strings(columns, n_columns).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
pub extern "C" fn df_csv_read_options_get_table_partition_columns(
    options: &mut DFCSVReadOptions,
    n_columns: *mut usize,
) -> *mut *mut libc::c_char {
    strings_to_c_strings(&options.options.table_partition_cols, n_columns)
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_register_csv(
    context: &mut DFSessionContext,
    name: *const libc::c_char,
    url: *const libc::c_char,
    options: Option<&mut DFCSVReadOptions>,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        let rs_name = c_string_to_str(name).into_df_error(error, None)?;
        let rs_url = c_string_to_str(url).into_df_error(error, None)?;
        let rs_options = match options {
            Some(o) => o.options.clone(),
            None => CsvReadOptions::default(),
        };
        let result = context.context.register_csv(rs_name, rs_url, rs_options);
        block_on(result).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

pub struct DFParquetReadOptions<'a> {
    options: ParquetReadOptions<'a>,
}

impl<'a> DFParquetReadOptions<'a> {
    pub fn new() -> Self {
        let options = ParquetReadOptions::default();
        Self { options }
    }
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_new<'a>() -> Box<DFParquetReadOptions<'a>> {
    Box::new(DFParquetReadOptions::new())
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_free(
    _options: Option<Box<DFParquetReadOptions>>,
) {
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_set_file_extension(
    options: &mut DFParquetReadOptions,
    file_extension: *const libc::c_char,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        options.options.file_extension =
            c_string_to_str(file_extension).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_get_file_extension(
    options: &mut DFParquetReadOptions,
) -> *mut libc::c_char {
    strdup(options.options.file_extension)
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_set_table_partition_columns(
    options: &mut DFParquetReadOptions,
    columns: *const *const libc::c_char,
    n_columns: usize,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        options.options.table_partition_cols =
            c_strings_to_strings(columns, n_columns).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_get_table_partition_columns(
    options: &mut DFParquetReadOptions,
    n_columns: *mut usize,
) -> *mut *mut libc::c_char {
    strings_to_c_strings(&options.options.table_partition_cols, n_columns)
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_set_pruning(
    options: &mut DFParquetReadOptions,
    pruning: bool,
) {
    options.options.parquet_pruning = pruning;
}

#[no_mangle]
pub extern "C" fn df_parquet_read_options_get_pruning(
    options: &mut DFParquetReadOptions,
) -> bool {
    options.options.parquet_pruning
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_register_parquet(
    context: &mut DFSessionContext,
    name: *const libc::c_char,
    url: *const libc::c_char,
    options: Option<&mut DFParquetReadOptions>,
    error: *mut *mut DFError,
) -> bool {
    let option = || -> Option<bool> {
        let rs_name = c_string_to_str(name).into_df_error(error, None)?;
        let rs_url = c_string_to_str(url).into_df_error(error, None)?;
        let rs_options = match options {
            Some(o) => o.options.clone(),
            None => ParquetReadOptions::default(),
        };
        let result = context
            .context
            .register_parquet(rs_name, rs_url, rs_options);
        block_on(result).into_df_error(error, None)?;
        Some(true)
    }();
    option.unwrap_or(false)
}
