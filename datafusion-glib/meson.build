# Copyright 2022 Sutou Kouhei <kou@clear-code.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

sources = files(
  'csv-read-options.c',
  'data-frame.c',
  'error.c',
  'parquet-read-options.c',
  'session-context.c',
)

headers = files(
  'csv-read-options.h',
  'data-frame.h',
  'datafusion-glib.h',
  'error.h',
  'parquet-read-options.h',
  'session-context.h',
)

raw_headers = files(
  'csv-read-options-raw.h',
  'data-frame-raw.h',
  'datafusion-glib-raw.h',
  'parquet-read-options-raw.h',
  'session-context-raw.h',
)

version_h_conf = configuration_data()
version_h_conf.set('GDF_VERSION_MAJOR', version_major)
version_h_conf.set('GDF_VERSION_MINOR', version_minor)
version_h_conf.set('GDF_VERSION_MICRO', version_micro)
version_h = configure_file(input: 'version.h.in',
                           output: 'version.h',
                           configuration: version_h_conf)
headers += version_h

gnome = import('gnome')
enums = gnome.mkenums_simple('enums',
                             identifier_prefix: 'GDF',
                             install_dir: include_dir / 'datafusion-glib',
                             install_header: true,
                             sources: headers,
                             symbol_prefix: 'gdf')
enums_source = enums[0]
enums_header = enums[1]


install_headers(headers + raw_headers, subdir: 'datafusion-glib')


gobject = dependency('gobject-2.0')
dependencies = [
  datafusion,
  arrow_glib,
  gobject,
]
libdatafusion_glib = library('datafusion-glib',
                             dependencies: dependencies,
                             include_directories: include_directories('..'),
                             install: true,
                             sources: sources + headers + raw_headers + enums,
                             soversion: so_version,
                             version: library_version)
datafusion_glib = declare_dependency(dependencies: dependencies,
                                     link_with: libdatafusion_glib,
                                     sources: enums_header)

pkgconfig = import('pkgconfig')
pkgconfig.generate(libdatafusion_glib,
                   description: 'GLib API for DataFusion',
                   filebase: 'datafusion-glib',
                   name: 'DataFusion GLib',
                   requires: ['datafusion', 'arrow-glib', 'gobject-2.0'],
                   version: version)

if have_gi
  datafusion_glib_gir = \
    gnome.generate_gir(libdatafusion_glib,
                       dependencies: dependencies,
                       export_packages: 'datafusion-glib',
                       extra_args: [
                         '--warn-all',
                       ],
                       fatal_warnings: true,
                       header: 'datafusion-glib/datafusion-glib.h',
                       identifier_prefix: 'GDF',
                       includes: [
                         'Arrow-1.0',
                         'GObject-2.0',
                       ],
                       install: true,
                       namespace: 'DataFusion',
                       nsversion: api_version,
                       sources: sources + headers + enums,
                       symbol_prefix: 'gdf')

  if generate_vapi
    datafusion_glib_vapi = \
      gnome.generate_vapi('datafusion-glib',
                          gir_dirs: [
                            arrow_glib.get_variable('girdir'),
                          ],
                          install: true,
                          packages: ['arrow-glib', 'gobject-2.0'],
                          sources: [datafusion_glib_gir[0]],
                          vapi_dirs: [
                            arrow_glib.get_variable('vapidir'),
                          ])
  endif
endif
