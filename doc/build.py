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
import shutil
import subprocess

parser = argparse.ArgumentParser(description='Build documents')
parser.add_argument('--source-root',
                    action='store',
                    dest='source_root',
                    help='Source root directory',
                    metavar='PATH',
                    required=True)
parser.add_argument('--build-root',
                    action='store',
                    dest='build_root',
                    help='Build root directory',
                    metavar='PATH',
                    required=True)
parser.add_argument('--arrow-glib-gir-dir',
                    action='store',
                    dest='arrow_glib_gir_dir',
                    help='Path where Arrow-*.gir exists',
                    metavar='PATH',
                    required=True)
parser.add_argument('--source-reference',
                    action='store',
                    dest='source_reference',
                    help='Branch or tag name for the target source',
                    metavar='REFERENCE',
                    required=True)
args = parser.parse_args()
source_root = pathlib.Path(args.source_root).resolve()
build_root = pathlib.Path(args.build_root).resolve()
arrow_glib_gir_dir = pathlib.Path(args.arrow_glib_gir_dir).resolve()
source_reference = args.source_reference

datafusion_gir = next((build_root / 'datafusion-glib').glob('DataFusion-*.gir'))
build_doc_dir = build_root / 'doc'
gi_docgen_config = build_doc_dir / 'glib' / 'gi-docgen.toml'
html_dir = build_doc_dir / 'html'
sphinx_build_dir = build_doc_dir / 'build'

subprocess.run('doxygen', cwd=build_doc_dir, check=True)

sphinx_env = os.environ.copy()
sphinx_env['DOXYGEN_XML_DIR'] = str(build_doc_dir / 'xml')
sphinx_env['SWITCHER_VERSION'] = source_reference
subprocess.run(['make',
                '-f' + str(source_root / 'doc' / 'Makefile'),
                'SOURCEDIR=' + str(source_root / 'doc' / 'source'),
                f'BUILDDIR={sphinx_build_dir}',
                'html'],
               env=sphinx_env,
               check=True)

shutil.rmtree(html_dir, ignore_errors=True)
shutil.copytree(sphinx_build_dir / 'html', html_dir)

subprocess.run(['gi-docgen',
                'generate',
                f'--add-include-path={arrow_glib_gir_dir}',
                f'--config={gi_docgen_config}',
                f'--output-dir={html_dir}/glib',
                '--no-namespace-dir',
                datafusion_gir],
               check=True)
subprocess.run(['gi-docgen',
                'gen-index',
                f'--add-include-path={arrow_glib_gir_dir}',
                f'--config={gi_docgen_config}',
                f'--output-dir={html_dir}/glib',
                datafusion_gir],
               check=True)
