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

#include <datafusion-glib/csv-read-options-raw.h>
#include <datafusion-glib/error.h>

G_BEGIN_DECLS

/**
 * SECTION: csv-read-options
 * @include: datafusion-glib/datafusion-glib.h
 *
 * #GDFCSVReadOptions is a class for options on reading CSV.
 */

typedef struct GDFCSVReadOptionsPrivate_ {
  DFCSVReadOptions *options;
  GArrowSchema *schema;
  GArrowSchema *table_partition_columns;
  gchar *file_extension;
} GDFCSVReadOptionsPrivate;

enum {
  PROP_HAS_HEADER = 1,
  PROP_DELIMITER,
  PROP_SCHEMA_INFER_MAX_N_RECORDS,
};

G_DEFINE_TYPE_WITH_PRIVATE(GDFCSVReadOptions,
                           gdf_csv_read_options,
                           G_TYPE_OBJECT)

static void
gdf_csv_read_options_finalize(GObject *object)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(GDF_CSV_READ_OPTIONS(object));
  df_csv_read_options_free(priv->options);
  g_free(priv->file_extension);
  G_OBJECT_CLASS(gdf_csv_read_options_parent_class)->finalize(object);
}

static void
gdf_csv_read_options_dispose(GObject *object)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(GDF_CSV_READ_OPTIONS(object));
  if (priv->schema) {
    g_object_unref(priv->schema);
    priv->schema = NULL;
  }
  if (priv->table_partition_columns) {
    g_object_unref(priv->table_partition_columns);
    priv->table_partition_columns = NULL;
  }
  G_OBJECT_CLASS(gdf_csv_read_options_parent_class)->dispose(object);
}

static void
gdf_csv_read_options_set_property(GObject *object,
                                  guint prop_id,
                                  const GValue *value,
                                  GParamSpec *pspec)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(GDF_CSV_READ_OPTIONS(object));

  switch (prop_id) {
  case PROP_HAS_HEADER:
    df_csv_read_options_set_has_header(priv->options,
                                       g_value_get_boolean(value));
    break;
  case PROP_DELIMITER:
    df_csv_read_options_set_delimiter(priv->options, g_value_get_schar(value));
    break;
  case PROP_SCHEMA_INFER_MAX_N_RECORDS:
    df_csv_read_options_set_schema_infer_max_records(priv->options,
                                                     g_value_get_uint64(value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_csv_read_options_get_property(GObject *object,
                                 guint prop_id,
                                 GValue *value,
                                 GParamSpec *pspec)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(GDF_CSV_READ_OPTIONS(object));

  switch (prop_id) {
  case PROP_HAS_HEADER:
    g_value_set_boolean(value,
                        df_csv_read_options_get_has_header(priv->options));
    break;
  case PROP_DELIMITER:
    g_value_set_schar(value,
                      df_csv_read_options_get_delimiter(priv->options));
    break;
  case PROP_SCHEMA_INFER_MAX_N_RECORDS:
    g_value_set_uint64(
      value,
      df_csv_read_options_get_schema_infer_max_records(priv->options));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gdf_csv_read_options_init(GDFCSVReadOptions *object)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(GDF_CSV_READ_OPTIONS(object));
  priv->options = df_csv_read_options_new();
  priv->schema = NULL;
  char *extension = df_csv_read_options_get_file_extension(priv->options);
  priv->file_extension = g_strdup(extension);
  free(extension);
}

static void
gdf_csv_read_options_class_init(GDFCSVReadOptionsClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_csv_read_options_finalize;
  gobject_class->dispose = gdf_csv_read_options_dispose;
  gobject_class->set_property = gdf_csv_read_options_set_property;
  gobject_class->get_property = gdf_csv_read_options_get_property;

  DFCSVReadOptions *options = df_csv_read_options_new();

  GParamSpec *spec;
  /**
   * GDFCSVReadOptions:has-header:
   *
   * Whether the CSV file have a header or not.
   *
   * If schema inference is run on a file with no headers, default
   * column names are created.
   */
  spec = g_param_spec_boolean("has-header",
                              NULL,
                              NULL,
                              df_csv_read_options_get_has_header(options),
                              G_PARAM_READWRITE);
  g_object_class_install_property(gobject_class, PROP_HAS_HEADER, spec);

  /**
   * GDFCSVReadOptions:delimiter:
   *
   * An optional column delimiter. Defaults to `','`.
   */
  spec = g_param_spec_char("delimiter",
                           NULL,
                           NULL,
                           0,
                           G_MAXINT8,
                           df_csv_read_options_get_delimiter(options),
                           G_PARAM_READWRITE);
  g_object_class_install_property(gobject_class, PROP_DELIMITER, spec);

  /**
   * GDFCSVReadOptions:schema-infer-max-n-records:
   *
   * The max number of rows to read from CSV files for schema
   * inference if needed. Defaults to `1000`.
   */
  spec = g_param_spec_uint64("schema-infer-max-n-records",
                             NULL,
                             NULL,
                             0,
                             G_MAXUINT64,
                             df_csv_read_options_get_schema_infer_max_records(options),
                             G_PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_SCHEMA_INFER_MAX_N_RECORDS,
                                  spec);

  df_csv_read_options_free(options);
}

/**
 * gdf_csv_read_options_new:
 *
 * Returns: A new CSV read options.
 *
 * Since: 10.0.0
 */
GDFCSVReadOptions *
gdf_csv_read_options_new(void)
{
  return g_object_new(GDF_TYPE_CSV_READ_OPTIONS, NULL);
}

/**
 * gdf_csv_read_options_set_schema:
 * @options: A #GDFCSVReadOptions.
 * @schema: (nullable): A #GArrowSchema for read CSV.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_csv_read_options_set_schema(GDFCSVReadOptions *options,
                                GArrowSchema *schema,
                                GError **error)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  if (priv->schema == schema) {
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
    df_csv_read_options_set_schema(priv->options, c_abi_schema, &df_error);
  if (success) {
    if (priv->schema) {
      g_object_unref(priv->schema);
    }
    if (schema) {
      priv->schema = g_object_ref(schema);
    } else {
      priv->schema = NULL;
    }
  } else {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[csv-read-options][set-schema] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_csv_read_options_get_schema:
 * @options: A #GDFCSVReadOptions.
 *
 * Returns: (transfer none) (nullable): A #GArrowSchema to be used on
 *   reading CSV.
 *
 * Since: 10.0.0
 */
GArrowSchema *
gdf_csv_read_options_get_schema(GDFCSVReadOptions *options)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  return priv->schema;
}

/**
 * gdf_csv_read_options_set_file_extention:
 * @options: A #GDFCSVReadOptions.
 * @extension: An extension for CSV file.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_csv_read_options_set_file_extension(GDFCSVReadOptions *options,
                                        const gchar *extension,
                                        GError **error)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  if (!extension) {
    g_set_error(error,
                GDF_ERROR,
                GDF_ERROR_ARROW,
                "[csv-read-options][set-file-extension] must not NULL");
    return FALSE;
  }
  if (strcmp(priv->file_extension, extension) == 0) {
    return TRUE;
  }
  DFError *df_error = NULL;
  bool success =
    df_csv_read_options_set_file_extension(priv->options, extension, &df_error);
  if (success) {
    g_free(priv->file_extension);
    priv->file_extension = g_strdup(extension);
  } else {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[csv-read-options][set-file-extension] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_csv_read_options_get_file_extension:
 * @options: A #GDFCSVReadOptions.
 *
 * Returns: An extension for CSV to be read.
 *
 * Since: 10.0.0
 */
const gchar *
gdf_csv_read_options_get_file_extension(GDFCSVReadOptions *options)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  return priv->file_extension;
}

/**
 * gdf_csv_read_options_set_table_partition_columns:
 * @options: A #GDFCSVReadOptions.
 * @schema: Column names and their types that are used for table
 *   partition.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 10.0.0
 */
gboolean
gdf_csv_read_options_set_table_partition_columns(GDFCSVReadOptions *options,
                                                 GArrowSchema *schema,
                                                 GError **error)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
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
    df_csv_read_options_set_table_partition_columns(priv->options,
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
                "[csv-read-options][set-table-partition-columns] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

/**
 * gdf_csv_read_options_get_table_partition_columns:
 * @options: A #GDFCSVReadOptions.
 *
 * Returns: (transfer none) (nullable): The column names and their
 *   types that are used for table partition.
 *
 * Since: 10.0.0
 */
GArrowSchema *
gdf_csv_read_options_get_table_partition_columns(GDFCSVReadOptions *options)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  return priv->table_partition_columns;
}

DFCSVReadOptions *
gdf_csv_read_options_get_raw(GDFCSVReadOptions *options)
{
  GDFCSVReadOptionsPrivate *priv =
    gdf_csv_read_options_get_instance_private(options);
  return priv->options;
}
