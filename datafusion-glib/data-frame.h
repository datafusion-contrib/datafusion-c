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

#include <datafusion-glib/error.h>

G_BEGIN_DECLS

#define GDF_TYPE_DATA_FRAME (gdf_data_frame_get_type())
G_DECLARE_DERIVABLE_TYPE(GDFDataFrame,
                         gdf_data_frame,
                         GDF,
                         DATA_FRAME,
                         GObject)
struct _GDFDataFrameClass
{
  GObjectClass parent_class;
};

GDF_AVAILABLE_IN_8_0
gboolean
gdf_data_frame_show(GDFDataFrame *data_frame, GError **error);

G_END_DECLS
