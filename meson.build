# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

project('datafusion-c', 'rust', 'c',
        default_options: [
          'c_std=c99',
        ],
        license: 'Apache-2.0',
        version: '8.0.0')

version_numbers = meson.project_version().split('.')
version_major = version_numbers[0].to_int()
version_minor = version_numbers[1].to_int()
version_micro = version_numbers[2].to_int()

api_version = '@0@.@1@'.format(version_major, version_minor)
so_version = version_major * 100 + version_minor
so_version_patch = version_micro
library_version = '@0@.@1@.@2@'.format(so_version, so_version_patch, 0)

prefix = get_option('prefix')
include_dir = join_paths(prefix, get_option('includedir'))
data_dir = join_paths(prefix, get_option('datadir'))

build_type = get_option('buildtype')
libdatafusion_command = [
  'cargo',
  'build',
  '--manifest-path',
  '@INPUT0@',
  '--target-dir',
  '@OUTDIR@',
  '--out-dir',
  '@OUTDIR@',
  '-Z',
  'unstable-options',
]
if build_type == 'release'
  libdatafusion_command += ['--release']
endif
if build_machine.system() == 'darwin'
  libdatafusion_so = 'libdatafusion.dylib'
elif build_machine.system() == 'windows'
  libdatafusion_so = 'datafusion.dll'
else
  libdatafusion_so = 'libdatafusion.so'
endif
libdatafusion = custom_target('libdatafusion',
                              build_by_default: true,
                              command: libdatafusion_command,
                              input: [
                                'Cargo.toml',
                                'src/lib.rs',
                              ],
                              install: true,
                              install_dir: get_option('libdir'),
                              output: libdatafusion_so)
datafusion = declare_dependency(
  link_with: libdatafusion,
  include_directories: include_directories('include')
)
install_headers(files('include/datafusion.h'))

have_gi = dependency('gobject-introspection-1.0', required: false).found()
generate_vapi = have_gi and get_option('vala')
if generate_vapi
  add_languages('vala')
endif

subdir('datafusion-glib')
subdir('examples')
if get_option('doc')
  subdir('doc/datafusion-glib')
endif
