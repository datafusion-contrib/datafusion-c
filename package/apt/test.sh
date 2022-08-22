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

if [ $# -lt 1 ]; then
  echo "Usage: $0 rc"
  echo "       $0 staging-rc"
  echo "       $0 release"
  echo "       $0 staging-release"
  echo "       $0 local"
  echo " e.g.: $0 rc              # Verify RC"
  echo " e.g.: $0 staging-rc      # Verify RC on staging"
  echo " e.g.: $0 release         # Verify release"
  echo " e.g.: $0 staging-release # Verify release on staging"
  echo " e.g.: $0 local           # Verify packages on local"
  exit 1
fi

TYPE="$1"

echo "::group::Prepare repository"

export DEBIAN_FRONTEND=noninteractive

APT_INSTALL="apt install -y -V --no-install-recommends"

apt update
${APT_INSTALL} ca-certificates lsb-release wget

code_name=$(lsb_release --codename --short)
distribution=$(lsb_release --id --short | tr A-Z a-z)
architecture=$(dpkg --print-architecture)

artifactory_base_url="https://apache.jfrog.io/artifactory/arrow/${distribution}"
case ${TYPE} in
  rc|staging-rc|staging-release)
    suffix=-${TYPE%-release}
    artifactory_base_url+="${suffix}"
    ;;
esac

wget ${artifactory_base_url}/apache-arrow-apt-source-latest-${code_name}.deb
${APT_INSTALL} ./apache-arrow-apt-source-latest-${code_name}.deb
case ${TYPE} in
  rc|staging-rc|staging-release)
    sed -i'' \
        -e "s,/${distribution}/,/${distribution}${suffix}/,g" \
        /etc/apt/sources.list.d/apache-arrow.sources
    ;;
esac
apt update

if [[ ${TYPE} == "local" ]]; then
  repositories_dir=/host/package/apt/repositories
  ${APT_INSTALL} \
    ${repositories_dir}/${distribution}/pool/${code_name}/*/*/*/*_${architecture}.deb
else
  ${APT_INSTALL} libdatafusion-glib-dev
fi
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
cp -a /host/example/sql-glib.vala build/vala/
pushd build/vala/
valac --pkg datafusion-glib --pkg posix sql-glib.vala
./sql-glib
popd

${APT_INSTALL} ruby-dev rubygems-integration
gem install gobject-introspection
ruby -r gi -e "p GI.load('DataFusion')"
echo "::endgroup"
