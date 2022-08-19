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
 * @columns: (array length=n_columns): Column names that are used for table
 *   partition.
 * @n_columns: The number of column names.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_parquet_read_options_set_table_partition_columns(
  GDFParquetReadOptions *options,
  const gchar **columns,
  gsize n_columns,
  GError **error)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  DFError *df_error = NULL;
  bool success =
    df_parquet_read_options_set_table_partition_columns(priv->options,
                                                        columns,
                                                        n_columns,
                                                        &df_error);
  if (!success) {
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
 * Returns: (transfer full) (nullable): The column names that are used for
 *   table partition.
 *
 *   It should be freed by g_strfreev() when no longer needed.
 *
 * Since: 10.0.0
 */
gchar **
gdf_parquet_read_options_get_table_partition_columns(
  GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  uintptr_t df_n_columns = 0;
  char **df_columns =
    df_parquet_read_options_get_table_partition_columns(priv->options,
                                                        &df_n_columns);
  if (df_n_columns == 0) {
    return NULL;
  }
  gchar **columns = g_new(gchar *, df_n_columns + 1);
  uintptr_t i;
  for (i = 0; i < df_n_columns; i++) {
    columns[i] = g_strdup(df_columns[i]);
    free(df_columns[i]);
  }
  columns[i] = NULL;
  free(df_columns);
  return columns;
}

DFParquetReadOptions *
gdf_parquet_read_options_get_raw(GDFParquetReadOptions *options)
{
  GDFParquetReadOptionsPrivate *priv =
    gdf_parquet_read_options_get_instance_private(options);
  return priv->options;
}
