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

#include <datafusion-glib/data-frame-raw.h>
#include <datafusion-glib/error.h>

G_BEGIN_DECLS

/**
 * SECTION: data-frame
 * @include: datafusion-glib/datafusion-glib.h
 *
 * #GDFParquertWriterProperties is a class to customize how to write
 * an Apache Parquet file.
 *
 * #GDFDataFrame is a data frame class.
 */

typedef struct GDFParquetWriterPropertiesPrivate_ {
  DFParquetWriterProperties *properties;
} GDFParquetWriterPropertiesPrivate;

enum {
  PROP_PARQUET_WRITER_PROPERTIES = 1,
};

G_DEFINE_TYPE_WITH_PRIVATE(GDFParquetWriterProperties,
                           gdf_parquet_writer_properties,
                           G_TYPE_OBJECT)

static void
gdf_parquet_writer_properties_finalize(GObject *object)
{
  GDFParquetWriterPropertiesPrivate *priv =
    gdf_parquet_writer_properties_get_instance_private(
      GDF_PARQUET_WRITER_PROPERTIES(object));
  df_parquet_writer_properties_free(priv->properties);
  G_OBJECT_CLASS(gdf_parquet_writer_properties_parent_class)->finalize(object);
}

static void
gdf_parquet_writer_properties_init(GDFParquetWriterProperties *object)
{
  GDFParquetWriterPropertiesPrivate *priv =
    gdf_parquet_writer_properties_get_instance_private(
      GDF_PARQUET_WRITER_PROPERTIES(object));
  priv->properties = df_parquet_writer_properties_new();
}

static void
gdf_parquet_writer_properties_class_init(GDFParquetWriterPropertiesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = gdf_parquet_writer_properties_finalize;
}

/**
 * gdf_parquet_writer_properties_new:
 *
 * Returns: A new Apache Parquet writer properties.
 *
 * Since: 21.0.0
 */
GDFParquetWriterProperties *
gdf_parquet_writer_properties_new(void)
{
  return g_object_new(GDF_TYPE_PARQUET_WRITER_PROPERTIES, NULL);
}

/**
 * gdf_parquet_writer_properties_set_max_row_group_size:
 * @properties: A #GDFParquetWriterProperties.
 * @size: The maximum number of rows in a row group.
 *
 * Set the maximum number of rows in a row group.
 *
 * Since: 21.0.0
 */
void
gdf_parquet_writer_properties_set_max_row_group_size(
  GDFParquetWriterProperties *properties,
  guint64 size)
{
  GDFParquetWriterPropertiesPrivate *priv =
    gdf_parquet_writer_properties_get_instance_private(properties);
  df_parquet_writer_properties_set_max_row_group_size(priv->properties, size);
}


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
 * Since: 10.0.0
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

/**
 * gdf_data_frame_to_table:
 * @data_frame: A #GDFDataFrame.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: (transfer full): A #GArrowTable of this data frame, %NULL on error.
 *
 * Since: 10.0.0
 */
GArrowTable *
gdf_data_frame_to_table(GDFDataFrame *data_frame, GError **error)
{
  GDFDataFramePrivate *priv = gdf_data_frame_get_instance_private(data_frame);
  DFArrowSchema *c_abi_schema;
  DFArrowArray **c_abi_record_batches;
  DFError *df_error = NULL;
  gint64 n = df_data_frame_export(priv->data_frame,
                                  &c_abi_schema,
                                  &c_abi_record_batches,
                                  &df_error);
  if (n < 0) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[data-frame][to-table] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
    return NULL;
  }

  GArrowSchema *schema = garrow_schema_import(c_abi_schema, error);
  GArrowRecordBatch **record_batches = g_newa(GArrowRecordBatch *, n);
  gint64 i;
  for (i = 0; i < n; i++) {
    record_batches[i] = garrow_record_batch_import(c_abi_record_batches[i],
                                                   schema,
                                                   error);
  }
  free(c_abi_record_batches);
  GArrowTable *table =
    garrow_table_new_record_batches(schema, record_batches, n, error);
  for (i = 0; i < n; i++) {
    g_object_unref(record_batches[i]);
  }
  g_object_unref(schema);
  return table;
}

/**
 * gdf_data_frame_write_parquet:
 * @data_frame: A #GDFDataFrame.
 * @path: An output path.
 * @properties: (nullable): Properties how to write Apache Parquet files.
 * @error: (nullable): Return location for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 21.0.0
 */
gboolean
gdf_data_frame_write_parquet(GDFDataFrame *data_frame,
                             const gchar *path,
                             GDFParquetWriterProperties *properties,
                             GError **error)
{
  GDFDataFramePrivate *priv = gdf_data_frame_get_instance_private(data_frame);
  DFParquetWriterProperties *df_properties = NULL;
  if (properties) {
    df_properties = gdf_parquet_writer_properties_get_raw(properties);
  }
  DFError *df_error = NULL;
  gboolean success = df_data_frame_write_parquet(priv->data_frame,
                                                 path,
                                                 df_properties,
                                                 &df_error);
  if (!success) {
    g_set_error(error,
                GDF_ERROR,
                df_error_get_code(df_error),
                "[data-frame][write-parquet] %s",
                df_error_get_message(df_error));
    df_error_free(df_error);
  }
  return success;
}

GDFDataFrame *
gdf_data_frame_new_raw(DFDataFrame *data_frame)
{
  return g_object_new(GDF_TYPE_DATA_FRAME,
                      "data-frame", data_frame,
                      NULL);
}

DFParquetWriterProperties *
gdf_parquet_writer_properties_get_raw(GDFParquetWriterProperties *properties)
{
  GDFParquetWriterPropertiesPrivate *priv =
    gdf_parquet_writer_properties_get_instance_private(properties);
  return priv->properties;
}
