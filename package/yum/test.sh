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

distribution=$(. /etc/os-release && echo "${ID}")
distribution_version=$(. /etc/os-release && echo "${VERSION_ID}" | grep -o "^[0-9]*")

artifactory_base_url="https://apache.jfrog.io/artifactory/arrow/${distribution}"
case ${TYPE} in
  rc|staging-rc|staging-release)
    suffix=-${TYPE%-release}
    artifactory_base_url+="${suffix}"
    ;;
esac
dnf install -y ${artifactory_base_url}/${distribution_version}/apache-arrow-release-latest.rpm
case ${TYPE} in
  rc|staging-rc|staging-release)
    sed -i'' \
        -e "s,/${distribution}/,/${distribution}${suffix}/,g" \
        /etc/yum.repos.d/Apache-Arrow.repo
    ;;
esac

case ${distribution_version} in
  8)
    DNF_INSTALL="dnf install -y --enablerepo=powertools"
    ;;
  *)
    DNF_INSTALL="dnf install -y --enablerepo=crb"
    ;;
esac

# TODO: This is a workaround to install datafusion-glib-devel.
# We can remove this when Apache Arrow 10.0.0 is released.
${DNF_INSTALL} \
  arrow-glib-devel

if [[ ${TYPE} == "local" ]]; then
  repositories_dir=/host/package/yum/repositories/
  ${DNF_INSTALL} \
    ${repositories_dir}/${distribution}/${distribution_version}/$(arch)/Packages/*.rpm
else
  ${DNF_INSTALL} datafusion-glib-devel
fi
echo "::endgroup::"

echo "::group::Test DataFusion C"
${DNF_INSTALL} \
  gcc \
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

${DNF_INSTALL} vala
mkdir -p build/vala/
cp -a /host/example/sql-glib.vala build/vala/
pushd build/vala/
valac --pkg datafusion-glib --pkg posix sql-glib.vala
./sql-glib
popd

${DNF_INSTALL} ruby-devel redhat-rpm-config
MAKEFLAGS="-j$(nproc)" gem install gobject-introspection
ruby -r gi -e "p GI.load('DataFusion')"
echo "::endgroup"
