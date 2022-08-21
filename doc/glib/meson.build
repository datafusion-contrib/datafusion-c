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

package_id = 'datafusion-glib'
package_name = 'DataFusion GLib'
entities_conf = configuration_data()
entities_conf.set('PACKAGE', package_id)
entities_conf.set('PACKAGE_BUGREPORT',
                  'https://github.com/datafusion-contrib/datafusion-c/issues')
entities_conf.set('PACKAGE_NAME', package_name)
entities_conf.set('PACKAGE_STRING',
                  ' '.join([package_name, version]))
entities_conf.set('PACKAGE_URL',
                  'https://github.com/datafusion-contrib/datafusion-c')
entities_conf.set('PACKAGE_VERSION', version)
configure_file(input: 'entities.xml.in',
               output: 'entities.xml',
               configuration: entities_conf)

gi_docgen_toml_conf = configuration_data()
gi_docgen_toml_conf.set('SOURCE_REFERENCE', source_reference)
gi_docgen_toml_conf.set('VERSION', meson.project_version())
gi_docgen_toml = configure_file(input: 'gi-docgen.toml.in',
                                output: 'gi-docgen.toml',
                                configuration: gi_docgen_toml_conf)

glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_doc_path = glib_prefix / 'share' / 'gtk-doc' / 'html'
arrow_glib_prefix = dependency('arrow-glib').get_pkgconfig_variable('prefix')
arrow_glib_doc_path = arrow_glib_prefix / 'share' / 'gtk-doc' / 'html'
doc_path = data_dir / 'gtk-doc' / 'html' / package_id

dependencies = [
  datafusion_glib,
]
html_images = []
ignore_headers = [
  meson.source_root() / 'datafusion-glib' / 'csv-read-options-raw.h',
  meson.source_root() / 'datafusion-glib' / 'data-frame-raw.h',
  meson.source_root() / 'datafusion-glib' / 'session-context-raw.h',
]
source_directories = [
  meson.source_root() / package_id,
  meson.build_root() / package_id,
]
gnome.gtkdoc(package_id,
             dependencies: dependencies,
             fixxref_args: [
               '--html-dir=' + doc_path,
               '--extra-dir=' + (glib_doc_path / 'glib'),
               '--extra-dir=' + (glib_doc_path / 'gobject'),
               '--extra-dir=' + (arrow_glib_doc_path / 'arrow-glib'),
             ],
             gobject_typesfile: package_id + '.types',
             html_assets: html_images,
             ignore_headers: ignore_headers,
             install: true,
             main_xml: package_id + '-docs.xml',
             mkdb_args: [
               '--output-format=xml',
               '--name-space=gdf',
               '--source-suffixes=c,h',
             ],
             scan_args: [
               '--rebuild-types',
               '--deprecated-guards=GDF_DISABLE_DEPRECATED',
             ],
             src_dir: source_directories)
