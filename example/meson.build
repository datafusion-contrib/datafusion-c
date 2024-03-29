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

executable('sql-c', 'sql.c',
           dependencies: [datafusion])
executable('sql-glib-c', 'sql-glib.c',
           dependencies: [datafusion_glib],
           include_directories: include_directories('..'))

if generate_vapi
  vala_example_executable_kwargs = {
    'c_args': [
      '-I' + meson.build_root(),
      '-I' + meson.source_root(),
    ],
    'dependencies': [
      datafusion_glib_vapi,
      arrow_glib,
      gobject,
    ],
    'vala_args': [
      '--pkg', 'posix',
      '--vapidir', arrow_glib.get_variable('vapidir'),
    ],
  }
  executable('sql-glib-vala', 'sql-glib.vala',
             kwargs: vala_example_executable_kwargs)
endif

install_data('sql.c',
             'sql.py',
             'sql.rb',
             install_dir: data_dir / 'datafusion' / 'example')

install_data('sql-glib.c',
             'sql-glib.py',
             'sql-glib.rb',
             'sql-glib.vala',
             install_dir: data_dir / 'datafusion-glib' / 'example')
