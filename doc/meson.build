# Copyright 2022-2023 Sutou Kouhei <kou@clear-code.com>
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

subdir('glib')

doxyfile_conf = configuration_data()
doxyfile_conf.set('DATAFUSION_H', datafusion_h.full_path())
doxyfile = configure_file(input: 'Doxyfile.in',
                          output: 'Doxyfile',
                          configuration: doxyfile_conf)

depend_files = files(
  '..' / 'cbindgen.toml',
  'source' / 'conf.py',
  'source' / 'example' / 'sql.rst',
  'source' / 'glib-api.rst',
  'source' / 'index.rst',
  'source' / 'install.rst',
  'source' / 'introduction.rst',
  'source' / 'news' / '10.0.0.md',
  'source' / 'news' / '21.0.0.md',
  'source' / 'news.rst',
  'source' / 'raw-c-api.rst',
)
depend_files += [
  doxyfile,
  gi_docgen_toml,
]

custom_target('html',
              command: [
                find_program('python3'),
                '@INPUT0@',
                '--source-root=@SOURCE_ROOT@',
                '--build-root=@BUILD_ROOT@',
                '--arrow-glib-gir-dir=' + arrow_glib.get_pkgconfig_variable('girdir'),
                '--source-reference=' + source_reference,
              ],
              depend_files: depend_files,
              depends: [datafusion_glib_gir],
              input: ['build.py'],
              output: ['html'])
