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
#include <datafusion-glib/session-context-raw.h>

G_BEGIN_DECLS

/**
 * SECTION: session-context
 * @include: datafusion-glib/datafusion-glib.h
 *
 * #GDFSessionContext is a class for entry point.
 */

typedef struct GDFSessionContextPrivate_ {
  DFSessionContext *context;
} GDFSessionContextPrivate;

enum {
  PROP_SESSION_CONTEXT = 1,
};

G_DEFINE_TYPE_WITH_PRIVATE(GDFSessionContext,
                           gdf_session_context,
                           G_TYPE_OBJECT)

static void
gdf_session_context_finalize(GObject *object)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(GDF_SESSION_CONTEXT(object));
  df_session_context_free(priv->context);
  G_OBJECT_CLASS(gdf_session_context_parent_class)->finalize(object);
}

static void
gdf_session_context_set_property(GObject *object,
                                 guint prop_id,
                                 const GValue *value,
                                 GParamSpec *pspec)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(GDF_SESSION_CONTEXT(object));

  switch (prop_id) {
  case PROP_SESSION_CONTEXT:
    priv->context = g_value_get_pointer(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_session_context_init(GDFSessionContext *object)
{
}

static void
gdf_session_context_class_init(GDFSessionContextClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_session_context_finalize;
  gobject_class->set_property = gdf_session_context_set_property;

  GParamSpec *spec;
  spec = g_param_spec_pointer("session-context",
                              "Session context",
                              "The raw DFSessionContext *",
                              G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property(gobject_class, PROP_SESSION_CONTEXT, spec);
}

/**
 * gdf_session_context_new:
 *
 * Returns: A new session context.
 *
 * Since: 8.0.0
 */
GDFSessionContext *
gdf_session_context_new(void)
{
  return gdf_session_context_new_raw(df_session_context_new());
}

/**
 * gdf_session_context_sql:
 * @context: A #GDFSessionContext.
 * @sql: A SQL to be ran.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: (transfer full) (nullable): The executed result as
 *   #GDFDataFrame, %NULL on error.
 *
 * Since: 8.0.0
 */
GDFDataFrame *
gdf_session_context_sql(GDFSessionContext *context,
                        const gchar *sql,
                        GError **error)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(context);
  DFError *df_error = NULL;
  DFDataFrame *data_frame =
    df_session_context_sql(priv->context, sql, &df_error);
  if (df_error) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[session-context][sql] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
    return NULL;
  } else {
    return gdf_data_frame_new_raw(data_frame);
  }
}

GDFSessionContext *
gdf_session_context_new_raw(DFSessionContext *raw_context)
{
  return g_object_new(GDF_TYPE_SESSION_CONTEXT,
                      "session-context", raw_context,
                      NULL);
}
