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

set -exu

echo "::group::Prepare repository"

export DEBIAN_FRONTEND=noninteractive

APT_INSTALL="apt install -y -V --no-install-recommends"

apt update
${APT_INSTALL} ca-certificates lsb-release wget

code_name=$(lsb_release --codename --short)
distribution=$(lsb_release --id --short | tr A-Z a-z)
architecture=$(dpkg --print-architecture)

wget \
  https://apache.jfrog.io/artifactory/arrow/${distribution}/apache-arrow-apt-source-latest-${code_name}.deb
${APT_INSTALL} ./apache-arrow-apt-source-latest-${code_name}.deb
apt update

repositories_dir=/host/package/apt/repositories
${APT_INSTALL} \
  ${repositories_dir}/${distribution}/pool/${code_name}/*/*/*/*_${architecture}.deb
echo "::endgroup::"

echo "::group::Test DataFusion C"
${APT_INSTALL} \
  gcc \
  libc6-dev \
  make \
  pkg-config
mkdir -p build/c/
cp -a /host/example/sql.c build/c/
pushd build/c/
cc -o sql sql.c $(pkg-config --cflags --libs datafusion)
./sql
popd
echo "::endgroup::"

echo "::group::Test DataFusion GLib"
export G_DEBUG=fatal-warnings

${APT_INSTALL} valac
mkdir -p build/vala/
cp -a /host/example/sql.vala build/vala/
pushd build/vala/
valac --pkg datafusion-glib --pkg posix sql.vala
./sql
popd

${APT_INSTALL} ruby-dev rubygems-integration
gem install gobject-introspection
ruby -r gi -e "p GI.load('DataFusion')"
echo "::endgroup"
