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

#include <arrow-glib/arrow-glib.h>

#include <datafusion-glib/csv-read-options.h>
#include <datafusion-glib/data-frame.h>
#include <datafusion-glib/parquet-read-options.h>

G_BEGIN_DECLS

#define GDF_TYPE_SESSION_CONTEXT (gdf_session_context_get_type())
G_DECLARE_DERIVABLE_TYPE(GDFSessionContext,
                         gdf_session_context,
                         GDF,
                         SESSION_CONTEXT,
                         GObject)
struct _GDFSessionContextClass
{
  GObjectClass parent_class;
};

GDF_AVAILABLE_IN_10_0
GDFSessionContext *
gdf_session_context_new(void);
GDF_AVAILABLE_IN_10_0
GDFDataFrame *
gdf_session_context_sql(GDFSessionContext *context,
                        const gchar *sql,
                        GError **error);
GDF_AVAILABLE_IN_10_0
gboolean
gdf_session_context_deregister(GDFSessionContext *context,
                               const gchar *name,
                               GError **error);
GDF_AVAILABLE_IN_10_0
gboolean
gdf_session_context_register_record_batch(GDFSessionContext *context,
                                          const gchar *name,
                                          GArrowRecordBatch *record_batch,
                                          GError **error);
GDF_AVAILABLE_IN_10_0
gboolean
gdf_session_context_register_table(GDFSessionContext *context,
                                   const gchar *name,
                                   GArrowTable *table,
                                   GError **error);

GDF_AVAILABLE_IN_10_0
gboolean
gdf_session_context_register_csv(GDFSessionContext *context,
                                 const gchar *name,
                                 const gchar *url,
                                 GDFCSVReadOptions *options,
                                 GError **error);

GDF_AVAILABLE_IN_10_0
gboolean
gdf_session_context_register_parquet(GDFSessionContext *context,
                                     const gchar *name,
                                     const gchar *url,
                                     GDFParquetReadOptions *options,
                                     GError **error);


G_END_DECLS
