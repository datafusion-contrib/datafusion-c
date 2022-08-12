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

#include <arrow-glib/arrow-glib.h>
#include <datafusion-glib/data-frame.h>

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

G_END_DECLS
