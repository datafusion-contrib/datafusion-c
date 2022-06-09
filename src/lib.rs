// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

use std::boxed::Box;
use std::ffi::CStr;
use std::ffi::CString;
use std::future::Future;
use std::sync::Arc;

use datafusion::common::DataFusionError;
use datafusion::dataframe::DataFrame;
use datafusion::execution::context::SessionContext;

#[repr(C)]
pub struct DFError {
    code: i32,
    message: *mut libc::c_char,
}

impl DFError {
    pub fn new(code: i32, message: *mut libc::c_char) -> Self {
        Self { code, message }
    }
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_error_new(code: i32, message: *const libc::c_char) -> *mut DFError {
    let error = DFError::new(code, unsafe { libc::strdup(message) });
    Box::into_raw(Box::new(error))
}

/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called for the same `error` multiple
/// times.
#[no_mangle]
pub unsafe extern "C" fn df_error_free(error: *mut DFError) {
    libc::free((*error).message as *mut libc::c_void);
    Box::from_raw(error);
}

/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called with `error` that is freed by
/// `df_error_free()`.
#[no_mangle]
pub unsafe extern "C" fn df_error_get_message(
    error: *mut DFError,
) -> *const libc::c_char {
    (*error).message
}

/// # Safety
///
/// This function should not be called with `error` that is not
/// created by `df_error_new()`.
///
/// This function should not be called with `error` that is freed by
/// `df_error_free()`.
#[no_mangle]
pub unsafe extern "C" fn df_error_get_code(error: *mut DFError) -> i32 {
    (*error).code
}

trait IntoDFError {
    type Value;
    fn into_df_error(
        self,
        error: *mut *mut DFError,
        error_value: Option<Self::Value>,
    ) -> Option<Self::Value>;
}

#[allow(clippy::upper_case_acronyms)]
enum DFErrorCode {
    Arrow,
    Parquet,
    #[allow(dead_code)]
    Avro,
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

fn df_error_set(error: *mut *mut DFError, code: i32, message: &str) {
    if error.is_null() {
        return;
    }
    let c_string_message = match CString::new(message) {
        Ok(c_string_message) => c_string_message,
        Err(_) => return,
    };
    unsafe {
        *error = df_error_new(code, c_string_message.as_ptr());
    };
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
                df_error_set(error, code as i32, &e.to_string());
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
                df_error_set(error, DFErrorCode::External as i32, &e.to_string());
                error_value
            }
        }
    }
}

fn block_on<F: Future>(future: F) -> F::Output {
    tokio::runtime::Runtime::new().unwrap().block_on(future)
}

#[repr(C)]
pub struct DFDataFrame {
    data_frame: Arc<DataFrame>,
}

impl DFDataFrame {
    pub fn new(data_frame: Arc<DataFrame>) -> Self {
        Self { data_frame }
    }
}

/// # Safety
///
/// This function should not be called for the same `data_frame`
/// multiple times.
#[no_mangle]
pub unsafe extern "C" fn df_data_frame_free(data_frame: *mut DFDataFrame) {
    Box::from_raw(data_frame);
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_data_frame_show(
    data_frame: *mut DFDataFrame,
    error: *mut *mut DFError,
) {
    let future = unsafe { (*data_frame).data_frame.show() };
    block_on(future).into_df_error(error, None);
}

#[no_mangle]
pub extern "C" fn df_session_context_new() -> *mut SessionContext {
    let context = SessionContext::new();
    Box::into_raw(Box::new(context))
}

/// # Safety
///
/// This function should not be called with `context` that is not
/// created by `df_session_context_new()`.
///
/// This function should not be called for the same `context`
/// multiple times.
#[no_mangle]
pub unsafe extern "C" fn df_session_context_free(context: *mut SessionContext) {
    Box::from_raw(context);
}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub extern "C" fn df_session_context_sql(
    context: *mut SessionContext,
    sql: *const libc::c_char,
    error: *mut *mut DFError,
) -> *mut DFDataFrame {
    let cstr_sql = unsafe { CStr::from_ptr(sql) };
    let maybe_rs_sql = cstr_sql.to_str().into_df_error(error, None);
    let rs_sql = match maybe_rs_sql {
        Some(rs_sql) => rs_sql,
        None => return std::ptr::null_mut(),
    };
    let result = block_on(unsafe { (*context).sql(rs_sql) });
    let maybe_data_frame = result.into_df_error(error, None);
    match maybe_data_frame {
        Some(data_frame) => Box::into_raw(Box::new(DFDataFrame::new(data_frame))),
        None => std::ptr::null_mut(),
    }
}
