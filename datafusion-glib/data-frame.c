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

#include <datafusion-glib/data-frame-raw.h>

G_BEGIN_DECLS

/**
 * SECTION: data-frame
 * @include: datafusion-glib/datafusion-glib.h
 *
 * #GDFDataFrame is a data frame class.
 */

typedef struct GDFDataFramePrivate_ {
  DFDataFrame *data_frame;
} GDFDataFramePrivate;

enum {
  PROP_DATA_FRAME = 1,
};

G_DEFINE_TYPE_WITH_PRIVATE(GDFDataFrame,
                           gdf_data_frame,
                           G_TYPE_OBJECT)

static void
gdf_data_frame_finalize(GObject *object)
{
  GDFDataFramePrivate *priv =
    gdf_data_frame_get_instance_private(GDF_DATA_FRAME(object));
  df_data_frame_free(priv->data_frame);
  G_OBJECT_CLASS(gdf_data_frame_parent_class)->finalize(object);
}

static void
gdf_data_frame_set_property(GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
  GDFDataFramePrivate *priv =
    gdf_data_frame_get_instance_private(GDF_DATA_FRAME(object));

  switch (prop_id) {
  case PROP_DATA_FRAME:
    priv->data_frame = g_value_get_pointer(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_data_frame_init(GDFDataFrame *object)
{
}

static void
gdf_data_frame_class_init(GDFDataFrameClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_data_frame_finalize;
  gobject_class->set_property = gdf_data_frame_set_property;

  GParamSpec *spec;
  spec = g_param_spec_pointer("data-frame",
                              "Data frame",
                              "The raw DFDataFrame *",
                              G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property(gobject_class, PROP_DATA_FRAME, spec);
}

/**
 * gdf_data_frame_show:
 * @data_frame: A #GDFDataFrame.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Shows @data_frame content to standard output.
 *
 * Since: 8.0.0
 */
gboolean
gdf_data_frame_show(GDFDataFrame *data_frame, GError **error)
{
  GDFDataFramePrivate *priv = gdf_data_frame_get_instance_private(data_frame);
  DFError *df_error = NULL;
  df_data_frame_show(priv->data_frame, &df_error);
  if (df_error) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[data-frame][show] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
    return FALSE;
  } else {
    return TRUE;
  }
}

GDFDataFrame *
gdf_data_frame_new_raw(DFDataFrame *data_frame)
{
  return g_object_new(GDF_TYPE_DATA_FRAME,
                      "data-frame", data_frame,
                      NULL);
}
