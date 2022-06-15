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

version = meson.project_version()
version_numbers = version.split('.')
version_major = version_numbers[0].to_int()
version_minor = version_numbers[1].to_int()
version_micro = version_numbers[2].to_int()

api_version = '@0@.@1@'.format(version_major, version_minor)
so_version = '@0@'.format(version_major)
library_version = version

prefix = get_option('prefix')
data_dir = get_option('datadir')
include_dir = get_option('includedir')
lib_dir = get_option('libdir')
pkg_config_dir = lib_dir / 'pkgconfig'
full_data_dir = prefix / data_dir
full_include_dir = prefix / include_dir
full_lib_dir = prefix / lib_dir
full_pkg_config_dir = prefix / pkg_config_dir

rustc_cfg_result = run_command(find_program('rustc'), '--print=cfg',
                               capture: true,
                               check: true)
rustc_configs = {}
foreach rustc_config : rustc_cfg_result.stdout().split()
  if not rustc_config.startswith('target_')
    continue
  endif
  split_rustc_config = rustc_config.split('=')
  name = split_rustc_config[0]
  value = split_rustc_config[1]
  rustc_configs += {name : value.substring(1, -1)}
endforeach
build_type = get_option('buildtype')
libdatafusion_output_base_dir = '@0@-@1@-@2@-@3@'.format(
  rustc_configs['target_arch'],
  rustc_configs['target_vendor'],
  rustc_configs['target_os'],
  rustc_configs['target_env'],
)
libdatafusion_output_dir = libdatafusion_output_base_dir / build_type
if build_machine.system() == 'darwin'
  libdatafusion_so_path = 'libdatafusion.dylib'
  libdatafusion_so_install_dir = lib_dir
elif build_machine.system() == 'windows'
  libdatafusion_so_path = 'datafusion.dll'
  libdatafusion_so_install_dir = lib_dir
else
  libdatafusion_so_path = 'libdatafusion.so'
  libdatafusion_so_install_dir = false
endif
libdatafusion_output = [
  libdatafusion_so_path,
  'datafusion.h',
  'datafusion.pc',
  libdatafusion_output_base_dir,
]
libdatafusion_install_dir = [
  libdatafusion_so_install_dir,
  include_dir,
  pkg_config_dir,
  false,
]
if libdatafusion_so_path.endswith('.so')
  libdatafusion_output += [
    '@0@.@1@'.format(libdatafusion_so_path, version),
    '@0@.@1@'.format(libdatafusion_so_path, version_major),
  ]
  libdatafusion_install_dir += [
    lib_dir,
    false,
  ]
  install_symlink('@0@.@1@'.format(libdatafusion_so_path, version_major),
                  install_dir: lib_dir,
                  pointing_to: '@0@.@1@'.format(libdatafusion_so_path, version))
  install_symlink(libdatafusion_so_path,
                  install_dir: lib_dir,
                  pointing_to: '@0@.@1@'.format(libdatafusion_so_path, version_major))
endif
libdatafusion_command = [
  find_program('python3'),
  '@CURRENT_SOURCE_DIR@' / 'build.py',
  '--file', libdatafusion_output_dir / 'datafusion.pc',
  '--file', libdatafusion_output_dir / 'include' / 'datafusion.h',
  '--shared-object', libdatafusion_output_dir / libdatafusion_so_path,
  '--version', version,
  '--',
  find_program('cargo'),
  'cbuild',
  '--color', 'always',
  '--includedir', '${prefix}/@0@'.format(include_dir),
  '--libdir', '${exec_prefix}/@0@'.format(lib_dir),
  '--manifest-path', '@INPUT0@',
  '--prefix', prefix,
  '--target-dir','@OUTDIR@',
]
if build_type == 'release'
  libdatafusion_command += ['--release']
endif
libdatafusion = custom_target('libdatafusion',
                              build_by_default: true,
                              command: libdatafusion_command,
                              input: [
                                'Cargo.toml',
                                'src/capi.rs',
                                'src/lib.rs',
                              ],
                              install: true,
                              install_dir: libdatafusion_install_dir,
                              output: libdatafusion_output)
# This doesn't work on macOS because of a Meson bug.
# See also: https://github.com/mesonbuild/meson/pull/10476
libdatafusion_so = libdatafusion[0]
datafusion_h = libdatafusion[1]
datafusion_pc = libdatafusion[2]
datafusion = declare_dependency(link_with: [libdatafusion_so],
                                sources: [datafusion_h])

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

test_run = find_program('test/run.sh')
test('unit test',
     test_run,
     env: [
       'BUILD=no'
     ])
