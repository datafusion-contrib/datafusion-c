# Copyright 2022-2023 Sutou Kouhei <kou@clear-code.com>
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

ARG FROM=debian:bullseye
FROM ${FROM}

RUN \
  echo "debconf debconf/frontend select Noninteractive" | \
    debconf-set-selections

RUN \
  echo 'APT::Install-Recommends "false";' > \
    /etc/apt/apt.conf.d/disable-install-recommends

RUN sed -i'' -e 's/main$/main contrib non-free/g' /etc/apt/sources.list

ENV \
  RUSTUP_HOME=/usr/local/rust \
  CARGO_HOME=/usr/local/rust

ARG DEBUG
RUN \
  quiet=$([ "${DEBUG}" = "yes" ] || echo "-qq") && \
  apt update ${quiet} && \
  apt install -y -V ${quiet} ca-certificates curl lsb-release && \
  curl \
    --fail \
    --location \
    --remote-name \
    https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
  apt install -y -V ${quiet} ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
  rm apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
  apt update ${quiet} && \
  apt install -y -V ${quiet} \
    build-essential \
    ccache \
    cmake \
    debhelper \
    devscripts \
    git \
    gtk-doc-tools \
    libarrow-glib-dev \
    libarrow-glib-doc \
    libgirepository1.0-dev \
    libglib2.0-doc \
    ninja-build \
    pkg-config \
    python3-pip \
    ruby-dev \
    tzdata \
    valac && \
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
  pip3 install --upgrade meson && \
  ln -s /usr/local/bin/meson /usr/bin/ && \
  MAKEFLAGS="-j$(nproc)" gem install red-parquet && \
  apt clean && \
  rm -rf /var/lib/apt/lists/*
