/*
 * Copyright 2022-2023 Sutou Kouhei <kou@clear-code.com>
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

#include <datafusion-glib/data-frame.h>

#include <datafusion.h>

G_BEGIN_DECLS

GDF_AVAILABLE_IN_10_0
GDFDataFrame *
gdf_data_frame_new_raw(DFDataFrame *raw_data_frame);

GDF_AVAILABLE_IN_21_0
DFParquetWriterProperties *
gdf_parquet_writer_properties_get_raw(GDFParquetWriterProperties *properties);

G_END_DECLS
