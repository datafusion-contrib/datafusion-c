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

#include <datafusion-glib/parquet-read-options-raw.h>
#include <datafusion-glib/error.h>

G_BEGIN_DECLS

/**
 * SECTION: parquet-read-options
 * @include: datafusion-glib/datafusion-glib.h
 *
 * #GDFParquetReadOptions is a class for options on reading Parquet.
 */

typedef struct GDFParquetReadOptionsPrivate_ {
  DFParquetReadOptions *options;
  GArrowSchema *table_partition_columns;
  gchar *file_extension;
} GDFParquetReadOptionsPrivate;

enum {
  PROP_PRUNING = 1,
};

G_DEFINE_TYPE_WITH_PRIVATE(GDFParquetReadOptions,
                           gdf_parquet_read_options,
                           G_TYPE_OBJECT)

static void
gdf_parquet_read_options_finalize(GObject *object)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(
      GDF_PARQUET_READ_OPTIONS(object));
  df_parquet_read_options_free(priv->options);
  g_free(priv->file_extension);
  G_OBJECT_CLASS(gdf_parquet_read_options_parent_class)->finalize(object);
}

static void
gdf_parquet_read_options_dispose(GObject *object)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(
      GDF_PARQUET_READ_OPTIONS(object));
  if (priv->table_partition_columns) {
    g_object_unref(priv->table_partition_columns);
    priv->table_partition_columns = NULL;
  }
  G_OBJECT_CLASS(gdf_parquet_read_options_parent_class)->dispose(object);
}

static void
gdf_parquet_read_options_set_property(GObject *object,
                                      guint prop_id,
                                      const GValue *value,
                                      GParamSpec *pspec)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(
      GDF_PARQUET_READ_OPTIONS(object));

  switch (prop_id) {
  case PROP_PRUNING:
    df_parquet_read_options_set_pruning(priv->options,
                                        g_value_get_boolean(value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_parquet_read_options_get_property(GObject *object,
                                      guint prop_id,
                                      GValue *value,
                                      GParamSpec *pspec)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(
      GDF_PARQUET_READ_OPTIONS(object));

  switch (prop_id) {
  case PROP_PRUNING:
    g_value_set_boolean(value,
                        df_parquet_read_options_get_pruning(priv->options));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_parquet_read_options_init(GDFParquetReadOptions *object)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(
      GDF_PARQUET_READ_OPTIONS(object));
  priv->options = df_parquet_read_options_new();
  char *extension = df_parquet_read_options_get_file_extension(priv->options);
  priv->file_extension = g_strdup(extension);
  free(extension);
}

static void
gdf_parquet_read_options_class_init(GDFParquetReadOptionsClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_parquet_read_options_finalize;
  gobject_class->dispose = gdf_parquet_read_options_dispose;
  gobject_class->set_property = gdf_parquet_read_options_set_property;
  gobject_class->get_property = gdf_parquet_read_options_get_property;

  DFParquetReadOptions *options = df_parquet_read_options_new();

  GParamSpec *spec;
  /**
   * GDFParquetReadOptions:pruning:
   *
   * Whether parquet reader using the predicate to prune data or
   * not. This overrides a configuration on #DFSessionContext.
   */
  spec = g_param_spec_boolean("pruning",
                              NULL,
                              NULL,
                              df_parquet_read_options_get_pruning(options),
                              G_PARAM_READWRITE);
  g_object_class_install_property(gobject_class, PROP_PRUNING, spec);

  df_parquet_read_options_free(options);
}

/**
 * gdf_parquet_read_options_new:
 *
 * Returns: A new Parquet read options.
 *
 * Since: 10.0.0
 */
GDFParquetReadOptions *
gdf_parquet_read_options_new(void)
{
  return g_object_new(GDF_TYPE_PARQUET_READ_OPTIONS, NULL);
}

/**
 * gdf_parquet_read_options_set_file_extention:
 * @options: A #GDFParquetReadOptions.
 * @extension: An extension for Parquet file.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_parquet_read_options_set_file_extension(GDFParquetReadOptions *options,
                                            const gchar *extension,
                                            GError **error)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  if (!extension) {
    g_set_error(error,
                GDF_ERROR,
                GDF_ERROR_ARROW,
                "[parquet-read-options][set-file-extension] must not NULL");
    return FALSE;
  }
  if (strcmp(priv->file_extension, extension) == 0) {
    return TRUE;
  }
  DFError *df_error = NULL;
  bool success =
    df_parquet_read_options_set_file_extension(priv->options,
                                               extension,
                                               &df_error);
  if (success) {
    g_free(priv->file_extension);
    priv->file_extension = g_strdup(extension);
  } else {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[parquet-read-options][set-file-extension] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_parquet_read_options_get_file_extension:
 * @options: A #GDFParquetReadOptions.
 *
 * Returns: An extension for Parquet to be read.
 *
 * Since: 10.0.0
 */
const gchar *
gdf_parquet_read_options_get_file_extension(GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  return priv->file_extension;
}

/**
 * gdf_parquet_read_options_set_table_partition_columns:
 * @options: A #GDFParquetReadOptions.
 * @schema: Column names and their types that are used for table
 *   partition.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_parquet_read_options_set_table_partition_columns(
  GDFParquetReadOptions *options,
  GArrowSchema *schema,
  GError **error)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  if (priv->table_partition_columns == schema) {
    return TRUE;
  }
  gpointer c_abi_schema = NULL;
  if (schema) {
    c_abi_schema = garrow_schema_export(schema, error);
    if (!c_abi_schema) {
      return FALSE;
    }
  }
  DFError *df_error = NULL;
  bool success =
    df_parquet_read_options_set_table_partition_columns(priv->options,
                                                        c_abi_schema,
                                                        &df_error);
  if (success) {
    if (priv->table_partition_columns) {
      g_object_unref(priv->table_partition_columns);
    }
    if (schema) {
      priv->table_partition_columns = g_object_ref(schema);
    } else {
      priv->table_partition_columns = NULL;
    }
  } else {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[parquet-read-options][set-table-partition-columns] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_parquet_read_options_get_table_partition_columns:
 * @options: A #GDFParquetReadOptions.
 *
 * Returns: (transfer none) (nullable): The column names and their
 *   types that are used for table partition.
 *
 * Since: 10.0.0
 */
GArrowSchema *
gdf_parquet_read_options_get_table_partition_columns(
  GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
      gdf_parquet_read_options_get_instance_private(options);
  return priv->table_partition_columns;
}

/**
 * gdf_parquet_read_options_unset_pruning:
 * @options: A #GDFParquetReadOptions.
 *
 * Unset pruning for the options. If the options doesn't have pruning,
 * value in session is used.
 *
 * Since: 21.0.0
 */
void
gdf_parquet_read_options_unset_pruning(GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
      gdf_parquet_read_options_get_instance_private(options);
  df_parquet_read_options_unset_pruning(priv->options);
}

/**
 * gdf_parquet_read_options_is_set_pruning:
 * @options: A #GDFParquetReadOptions.
 *
 * Returns: %TRUE when the options have pruning value, %FALSE otherwise.
 *
 * Since: 21.0.0
 */
gboolean
gdf_parquet_read_options_is_set_pruning(GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
      gdf_parquet_read_options_get_instance_private(options);
  return df_parquet_read_options_is_set_pruning(priv->options);
}

DFParquetReadOptions *
gdf_parquet_read_options_get_raw(GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  return priv->options;
}
