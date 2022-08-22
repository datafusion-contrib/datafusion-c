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
  echo "Usage: $0 rc"
  echo "       $0 staging-rc"
  echo "       $0 release"
  echo "       $0 staging-release"
  echo " e.g.: $0 rc              # Verify RC packages"
  echo " e.g.: $0 staging-rc      # Verify RC packages on staging"
  echo " e.g.: $0 release         # Verify release packages"
  echo " e.g.: $0 staging-release # Verify release packages on staging"
  exit 1
fi

verify_type="$1"

source_dir="$(dirname "$0")/../.."

pushd "${source_dir}"
for dir in package/{apt,yum}/*; do
  if [ ! -d "${dir}" ]; then
    continue
  fi
  base_name=${dir##*/}
  distribution=
  code_name=
  case ${base_name} in
    *-aarch64|*-arm64)
      ;;
    almalinux-*|debian-*|ubuntu-*)
      distribution=${base_name%-*}
      code_name=${base_name#*-}
      ;;
  esac
  if [ "${distribution}" = "" -o "#{code_name}" = "" ]; then
    continue
  fi
  package_type=$(basename "$(dirname "${dir}")")
  docker run \
         --rm \
         --volume "$PWD:/host" \
         -it \
         ${distribution}:${code_name} \
         /host/package/${package_type}/test.sh \
         "${verify_type}"
done
popd
