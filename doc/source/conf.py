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

import datetime
import os
import re

project = 'DataFusion C'
copyright = f'2022-{datetime.date.today().year}, Sutou Kouhei'
author = 'Sutou Kouhei'
with open(f'{os.path.dirname(__file__)}/../../Cargo.toml') as cargo_toml:
    version_re = re.compile('^version = "(.+?)"', re.MULTILINE)
    version = version_re.search(cargo_toml.read()).group(1)
release = version

extensions = [
    'breathe',
]

breathe_default_project = 'datafusion-c'
breathe_domain_by_extension = {'h' : 'c'}
breathe_projects = {'datafusion-c': os.environ.get('DOXYGEN_XML_DIR', '../xml')}

templates_path = ['_templates']
exclude_patterns = []

html_context = {
    'github_user': 'datafusion-contrib',
    'github_repo': 'datafusion-c',
    'github_version': 'main',
    'doc_path': 'doc/source',
}
html_theme = 'pydata_sphinx_theme'
html_theme_options = {
    'github_url': 'https://github.com/datafusion-contrib/datafusion-c',
    'navbar_start': [
        'navbar-logo',
        'version-switcher',
    ],
    'navbar_center': [],
    'switcher': {
        # 'json_url': '/_static/switcher.json',
        'json_url': 'https://datafusion-contrib.github.io/datafusion-c/latest/_static/switcher.json',
        'version_match': os.environ.get('SWITCHER_VERSION', version),
    },
    "use_edit_page_button": True,
}
html_sidebars = {
    '**': [
        'search-field',
        'navbar-nav',
        'sidebar-nav-bs',
    ],
}
html_static_path = ['_static']
