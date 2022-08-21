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
 */

/*
 * How to build:
 *   $ cc -o sql-glib sql-glib.c $(pkg-config --cflags --libs datafusion-glib)
 *
 * How to run:
 *   $ ./sql-glib
 *   +----------+
 *   | Int64(1) |
 *   +----------+
 *   | 1        |
 *   +----------+
 */

#include <datafusion-glib/datafusion-glib.h>

#include <stdlib.h>

int
main(void)
{
  GDFSessionContext *context = gdf_session_context_new();
  GError *error = NULL;
  GDFDataFrame *data_frame = gdf_session_context_sql(context,
                                                     "SELECT 1;",
                                                     &error);
  if (error) {
    g_print("failed to run SQL: %s\n", error->message);
    g_error_free(error);
    g_object_unref(context);
    return EXIT_FAILURE;
  }
  gdf_data_frame_show(data_frame, &error);
  if (error) {
    g_print("failed to show data frame: %s\n", error->message);
    g_error_free(error);
  }
  g_object_unref(data_frame);
  g_object_unref(context);
  return EXIT_SUCCESS;
}
