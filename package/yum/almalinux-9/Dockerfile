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

ARG FROM=almalinux:9
FROM ${FROM}

ENV \
  RUSTUP_HOME=/usr/local/rust \
  CARGO_HOME=/usr/local/rust

ARG DEBUG
RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "--quiet") && \
  dnf install -y ${quiet} epel-release && \
  dnf install -y ${quiet} https://apache.jfrog.io/artifactory/arrow/almalinux/$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)/apache-arrow-release-latest.rpm && \
  dnf install --enablerepo=crb -y ${quiet} \
    arrow-glib-devel \
    arrow-glib-doc \
    ccache \
    cmake \
    gcc \
    gcc-c++ \
    git \
    gobject-introspection-devel \
    gtk-doc \
    ninja-build \
    make \
    pkg-config \
    python3-pip \
    rpmdevtools \
    ruby-devel \
    tzdata \
    vala && \
  curl https://sh.rustup.rs -sSf | sh -s -- -y --no-modify-path && \
  ${RUSTUP_HOME}/bin/cargo install cargo-c && \
  for exe in ${RUSTUP_HOME}/bin/*; do \
    ( \
      echo '#!/bin/sh'; \
      echo "export RUSTUP_HOME=${RUSTUP_HOME}"; \
      echo "export CARGO_HOME=${CARGO_HOME}"; \
      echo 'exec ${RUSTUP_HOME}/bin/${0##*/} "$@"'; \
    ) > /usr/bin/${exe##*/} && \
    chmod +x /usr/bin/${exe##*/}; \
  done && \
  MAKEFLAGS="-j$(nproc)" gem install red-arrow && \
  dnf clean ${quiet} all
