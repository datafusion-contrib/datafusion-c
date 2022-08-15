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

#include <datafusion-glib/csv-read-options-raw.h>
#include <datafusion-glib/data-frame-raw.h>
#include <datafusion-glib/error.h>
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
  GHashTable *registered_record_batches;
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
gdf_session_context_dispose(GObject *object)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(GDF_SESSION_CONTEXT(object));
  if (priv->registered_record_batches) {
    g_hash_table_unref(priv->registered_record_batches);
    priv->registered_record_batches = NULL;
  }
  G_OBJECT_CLASS(gdf_session_context_parent_class)->dispose(object);
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
gdf_session_context_registered_record_batches_value_destroy(gpointer data)
{
  GList *record_batches = data;
  g_list_free_full(record_batches, g_object_unref);
}

static void
gdf_session_context_init(GDFSessionContext *object)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(object);
  priv->registered_record_batches =
    g_hash_table_new_full(g_str_hash,
                          g_str_equal,
                          g_free,
                          gdf_session_context_registered_record_batches_value_destroy);
}

static void
gdf_session_context_class_init(GDFSessionContextClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_session_context_finalize;
  gobject_class->dispose = gdf_session_context_dispose;
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
 * Since: 10.0.0
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
 * Since: 10.0.0
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

static void
df_arrow_schema_release(DFArrowSchema *schema)
{
  if (schema && schema->release) {
    schema->release(schema);
  }
}

static void
df_arrow_array_release(DFArrowArray *array)
{
  if (array && array->release) {
    array->release(array);
  }
}

static void
df_arrow_array_release_destroy(gpointer array)
{
  df_arrow_array_release(array);
}

/**
 * gdf_session_context_deregister:
 * @context: A #GDFSessionContext.
 * @name: A name to be deregistered.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Deregisters a registered table. If there isn't a registered table
 * associated with @name, this function does nothing.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_session_context_deregister(GDFSessionContext *context,
                               const gchar *name,
                               GError **error)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(context);
  DFError *df_error = NULL;
  bool success = df_session_context_deregister(priv->context, name, &df_error);
  if (success) {
    g_hash_table_remove(priv->registered_record_batches, name);
  } else {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[session-context][deregister] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_session_context_register_record_batch:
 * @context: A #GDFSessionContext.
 * @name: A name for the record batch in the context.
 * @record_batch: A record batch to be registered.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_session_context_register_record_batch(GDFSessionContext *context,
                                          const gchar *name,
                                          GArrowRecordBatch *record_batch,
                                          GError **error)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(context);
  gpointer c_abi_array = NULL;
  gpointer c_abi_schema = NULL;
  if (!garrow_record_batch_export(record_batch,
                                  &c_abi_array,
                                  &c_abi_schema,
                                  error)) {
    return false;
  }
  DFError *df_error = NULL;
  bool success =
    df_session_context_register_record_batches(priv->context,
                                               name,
                                               c_abi_schema,
                                               (DFArrowArray **)&c_abi_array,
                                               1,
                                               &df_error);
  if (df_error) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[session-context][register-record-batch] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
    df_arrow_schema_release(c_abi_schema);
    df_arrow_array_release(c_abi_array);
  } else {
    g_hash_table_insert(priv->registered_record_batches,
                        g_strdup(name),
                        g_list_prepend(NULL, g_object_ref(record_batch)));
  }
  return success;
}

/**
 * gdf_session_context_register_table:
 * @context: A #GDFSessionContext.
 * @name: A name for the record batch in the context.
 * @table: A table to be registered.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_session_context_register_table(GDFSessionContext *context,
                                   const gchar *name,
                                   GArrowTable *table,
                                   GError **error)
{
  const gchar *tag = "[session-context][register-table]";
  GDFSessionContextPrivate *priv =
      gdf_session_context_get_instance_private(context);

  bool success = false;
  GArrowTableBatchReader *reader = garrow_table_batch_reader_new(table);
  if (!reader) {
    g_set_error(error,
                GDF_ERROR,
                GDF_ERROR_ARROW,
                "%s failed to create table to record batches converter",
                tag);
    return false;
  }
  GPtrArray *c_abi_arrays = g_ptr_array_new();
  g_ptr_array_set_free_func(c_abi_arrays, df_arrow_array_release_destroy);
  GList *record_batches = NULL;
  while (true) {
    GError *read_error = NULL;
    GArrowRecordBatch *record_batch =
      garrow_record_batch_reader_read_next(GARROW_RECORD_BATCH_READER(reader),
                                           &read_error);
    if (record_batch) {
      record_batches = g_list_prepend(record_batches, record_batch);
    }
    if (read_error) {
      g_propagate_error(error, read_error);
      goto exit;
    }
    if (!record_batch) {
      break;
    }
    gpointer c_abi_array = NULL;
    if (!garrow_record_batch_export(record_batch, &c_abi_array, NULL, error)) {
      goto exit;
    }
    g_ptr_array_add(c_abi_arrays, c_abi_array);
  }
  g_object_unref(reader);
  reader = NULL;

  GArrowSchema *schema = garrow_table_get_schema(table);
  gpointer c_abi_schema = garrow_schema_export(schema, error);
  if (!c_abi_schema) {
    goto exit;
  }

  DFError *df_error = NULL;
  success = df_session_context_register_record_batches(
    priv->context,
    name,
    c_abi_schema,
    (DFArrowArray **)(c_abi_arrays->pdata),
    c_abi_arrays->len,
    &df_error);
  if (df_error) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "%s %s",
                tag,
                df_error_get_message(df_error));
    df_error_free(df_error);
    df_arrow_schema_release(c_abi_schema);
  } else {
    g_ptr_array_set_free_func(c_abi_arrays, NULL);
    g_hash_table_insert(priv->registered_record_batches,
                        g_strdup(name),
                        record_batches);
    record_batches = NULL;
  }

exit:
  g_ptr_array_free(c_abi_arrays, TRUE);
  g_list_free_full(record_batches, g_object_unref);
  if (reader) {
    g_object_unref(reader);
  }
  return success;
}

/**
 * gdf_session_context_register_csv:
 * @context: A #GDFSessionContext.
 * @name: A name for the CSV in the context.
 * @path: A path of the CSV to be registered.
 * @options: (nullable): A #GDFCSVReadOptions.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_session_context_register_csv(GDFSessionContext *context,
                                 const gchar *name,
                                 const gchar *path,
                                 GDFCSVReadOptions *options,
                                 GError **error)
{
  GDFSessionContextPrivate *priv =
    gdf_session_context_get_instance_private(context);
  DFCSVReadOptions *df_options = NULL;
  if (options) {
    df_options = gdf_csv_read_options_get_raw(options);
  }
  DFError *df_error = NULL;
  bool success =
    df_session_context_register_csv(priv->context,
                                    name,
                                    path,
                                    df_options,
                                    &df_error);
  if (!success) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[session-context][register-csv] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

GDFSessionContext *
gdf_session_context_new_raw(DFSessionContext *raw_context)
{
  return g_object_new(GDF_TYPE_SESSION_CONTEXT,
                      "session-context", raw_context,
                      NULL);
}
