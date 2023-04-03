#!/bin/bash
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

set -eux

if [ $# -lt 1 ]; then
  echo "Usage: $0 version"
  echo " e.g.: $0 11.0.0"
  exit 1
fi

version=$1
major_version=$(echo ${version} | grep -o -E '^[0-9]+')

source_dir="$(dirname "$0")/../.."

git fetch origin --tags

branch="prepare-${version}"
git branch -D ${branch} || :
git checkout -b ${branch} origin/main

pushd "${source_dir}"
sed -i'.bak' \
  -e "s/^version = \".*\"/version = \"${version}\"/" \
  Cargo.toml
rm Cargo.toml.bak
git add Cargo.toml

sed -i'.bak' \
  -e "s/^version = \'.*\'/version = \"${version}\"/" \
  meson.build
rm meson.build.bak
git add meson.build
popd

pushd "${source_dir}/package/debian/"
sed -i'.bak' -E \
  -e "s/libdatafusion[0-9]+/libdatafusion${major_version}/g" \
  -e "s/libdatafusion-glib[0-9]+/libdatafusion-glib${major_version}/g" \
  -e "s/gir1\.2-datafusion-[0-9]+/gir1.2-datafusion-${major_version}/g" \
  control
rm control.bak
git add control
git mv gir1.2-datafusion-*.install \
       gir1.2-datafusion-${major_version}.0.install
git mv libdatafusion-glib[[:alnum:]]*.install \
       libdatafusion-glib${major_version}.install
git mv libdatafusion[[:alnum:]]*.install \
       libdatafusion${major_version}.install
popd

pushd "${source_dir}/doc/"
latest_news_md=$(ls source/news/*.md | sort -n -r | head -n1)
sed \
  -e "s/^# .*\$/# ${version} - $(date +%Y-%m-%d)/" \
  ${latest_news_md} > source/news/${version}.md
git add source/news/${version}.md
sed -i'.bak' \
  -e "N; /^\.\. toctree::/ a \ \ \ news/${version}" \
  source/news.rst
rm source/news.rst.bak
git add source/news.rst
sed -i'.bak' \
  -e "\,^  'source' / 'news\.rst', i \ \ 'source' / 'news' / '${version}.md'," \
  meson.build
rm meson.build.bak
git add meson.build
popd

pushd "${source_dir}/package"
rake version:update
git add apt/debian/
git add yum/*.spec.in
popd
