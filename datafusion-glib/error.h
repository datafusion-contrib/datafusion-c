/*
 * Copyright 2022 Sutou Kouhei <kou@clear-code.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <glib-object.h>

#include <datafusion-glib/version.h>

G_BEGIN_DECLS

/**
 * GDFError:
 * @GDF_ERROR_ARROW: Error returned by arrow crate.
 * @GDF_ERROR_PARQUET: Error returned by parquet crate.
 * @GDF_ERROR_AVRO: Error returned by avro-rs crate.
 * @GDF_ERROR_OBJECT_STORE: Error returned by object_store crate.
 * @GDF_ERROR_IO: Error associated to I/O operations and associated traits.
 * @GDF_ERROR_SQL: Error returned when SQL is syntactically incorrect.
 * @GDF_ERROR_NOT_IMPLEMENTED: Error returned on a branch that we know it is
 *   possible but to which we still have no implementation for.
 *   Often, these errors are tracked in our issue tracker.
 * @GDF_ERROR_INTERNAL: Error returned as a consequence of an error in
 *   DataFusion. This error should not happen in normal usage of DataFusion.
 * @GDF_ERROR_PLAN: This error happens whenever a plan is not valid. Examples
 *   include impossible casts.
 * @GDF_ERROR_SCHEMA: This error happens with schema-related errors, such as
 *   schema inference not possible and non-unique column names.
 * @GDF_ERROR_EXECUTION: Error returned during execution of the query.
 *   Examples include files not found, errors in parsing certain types.
 * @GDF_ERROR_RESOURCES_EXHAUSTED: This error is thrown when a consumer cannot
 *   acquire memory from the Memory Manager we can just cancel the execution
 *   of the partition.
 * @GDF_ERROR_EXTERNAL: For example, a custom S3Error from the crate
 *   datafusion-objectstore-s3.
 * @GDF_ERROR_JIT: Error occurs during code generation.
 *
 * The error codes are used by all datafusion-glib functions.
 *
 * They are corresponding to `DFErrorCode` values.
 *
 * Since: 10.0.0
 */
typedef enum {
  GDF_ERROR_ARROW,
  GDF_ERROR_PARQUET,
  GDF_ERROR_AVRO,
  GDF_ERROR_OBJECT_STORE,
  GDF_ERROR_IO,
  GDF_ERROR_SQL,
  GDF_ERROR_NOT_IMPLEMENTED,
  GDF_ERROR_INTERNAL,
  GDF_ERROR_PLAN,
  GDF_ERROR_SCHEMA,
  GDF_ERROR_EXECUTION,
  GDF_ERROR_RESOURCES_EXHAUSTED,
  GDF_ERROR_EXTERNAL,
  GDF_ERROR_JIT,
} GDFError;

#define GDF_ERROR gdf_error_quark()

GQuark gdf_error_quark(void);

G_END_DECLS
