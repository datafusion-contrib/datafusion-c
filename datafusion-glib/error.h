/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

#include <glib-object.h>

#include <datafusion-glib/version.h>

G_BEGIN_DECLS

/**
 * GDFError:
 * @GDF_ERROR_ARROW:
 * @GDF_ERROR_PARQUET:
 * @GDF_ERROR_AVRO:
 * @GDF_ERROR_IO:
 * @GDF_ERROR_SQL:
 * @GDF_ERROR_NOT_IMPLEMENTED:
 * @GDF_ERROR_INTERNAL:
 * @GDF_ERROR_PLAN:
 * @GDF_ERROR_SCHEMA:
 * @GDF_ERROR_EXECUTION:
 * @GDF_ERROR_RESOURCES_EXHAUSTED:
 * @GDF_ERROR_EXTERNAL:
 * @GDF_ERROR_JIT:
 *
 * The error codes are used by all datafusion-glib functions.
 *
 * They are corresponding to `DFErrorCode` values.
 *
 * Since: 8.0.0
 */
typedef enum {
  GDF_ERROR_ARROW,
  GDF_ERROR_PARQUET,
  GDF_ERROR_AVRO,
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
