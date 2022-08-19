#!#/usr/bin/env python3
#
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

import argparse
import os
import pathlib
import platform
import shutil
import subprocess

parser = argparse.ArgumentParser(description='Build and copy artifacts '
                                 'to the current directory')
parser.add_argument('--version',
                    action='store',
                    dest='version',
                    help='Shared object version',
                    metavar='VERSION',
                    required=True)
parser.add_argument('--shared-object',
                    dest='shared_object',
                    help='Path to shared object',
                    metavar='SHARED_OBJECT',
                    required=True)
parser.add_argument('--file',
                    action='append',
                    dest='files',
                    help='Path to not shared object file '
                    '(can be used multiple times)',
                    metavar='DATA')
parser.add_argument('command_line', nargs='*')
args = parser.parse_args()


subprocess.run(args.command_line, check=True)


shared_object_base = os.path.basename(args.shared_object)
if platform.system() == 'Linux':
    version_major = args.version.split('.')[0]
    shutil.copy2(args.shared_object, f'{shared_object_base}.{args.version}')
    def ln_fs(src, dest):
        try:
            os.remove(dest)
        except FileNotFoundError:
            pass
        os.symlink(src, dest)
    ln_fs(f'{shared_object_base}.{args.version}',
          f'{shared_object_base}.{version_major}')
    ln_fs(f'{shared_object_base}.{version_major}',
          f'{shared_object_base}')
elif platform.system() == 'Darwin':
    version_major = args.version.split('.')[0]
    suffix = pathlib.PurePath(shared_object_base).suffix
    shared_object_base = pathlib.PurePath(shared_object_base).with_suffix('')
    shutil.copy2(args.shared_object,
                 f'{shared_object_base}.{version_major}{suffix}')
    def ln_fs(src, dest):
        try:
            os.remove(dest)
        except FileNotFoundError:
            pass
        os.symlink(src, dest)
    ln_fs(f'{shared_object_base}.{version_major}{suffix}',
          f'{shared_object_base}{suffix}')
else:
    shutil.copy2(args.shared_object, '.')

for file in args.files:
    shutil.copy2(file, '.')
