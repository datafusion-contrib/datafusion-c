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

#include <arrow-glib/arrow-glib.h>

#include <datafusion-glib/version.h>

G_BEGIN_DECLS

#define GDF_TYPE_PARQUET_READ_OPTIONS (gdf_parquet_read_options_get_type())
G_DECLARE_DERIVABLE_TYPE(GDFParquetReadOptions,
                         gdf_parquet_read_options,
                         GDF,
                         PARQUET_READ_OPTIONS,
                         GObject)
struct _GDFParquetReadOptionsClass
{
  GObjectClass parent_class;
};

GDF_AVAILABLE_IN_10_0
GDFParquetReadOptions *
gdf_parquet_read_options_new(void);

GDF_AVAILABLE_IN_10_0
gboolean
gdf_parquet_read_options_set_file_extension(GDFParquetReadOptions *options,
                                        const gchar *extension,
                                        GError **error);
GDF_AVAILABLE_IN_10_0
const gchar *
gdf_parquet_read_options_get_file_extension(GDFParquetReadOptions *options);

GDF_AVAILABLE_IN_10_0
gboolean
gdf_parquet_read_options_set_table_partition_columns(
  GDFParquetReadOptions *options,
  GArrowSchema *schema,
  GError **error);
GDF_AVAILABLE_IN_10_0
GArrowSchema *
gdf_parquet_read_options_get_table_partition_columns(
  GDFParquetReadOptions *options);

GDF_AVAILABLE_IN_21_0
void
gdf_parquet_read_options_unset_pruning(GDFParquetReadOptions *options);

GDF_AVAILABLE_IN_21_0
gboolean
gdf_parquet_read_options_is_set_pruning(GDFParquetReadOptions *options);


G_END_DECLS
