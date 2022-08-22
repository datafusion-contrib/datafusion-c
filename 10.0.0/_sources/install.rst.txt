.. Copyright 2022 Sutou Kouhei <kou@clear-code.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and

Install
=======

We provide binary packages for some platforms. If your platform isn't
included these platforms, you need to build by yourself.

Debian GNU/Linux and Ubuntu
---------------------------

Here are supported versions:

* Debian GNU/Linux bullseye
* Debian GNU/Linux bookworm
* Ubuntu 20.04 LTS
* Ubuntu 22.04 LTS

.. code-block:: console

   $ sudo apt update
   $ sudo apt install -y -V ca-certificates lsb-release wget
   $ wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
   $ sudo apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
   $ sudo apt update
   $ sudo apt install -y -V libdatafusion-dev # For C
   $ sudo apt install -y -V libdatafusion-glib-dev # For GLib

AlmaLinux 8 and Red Hat Enterprise Linux 8
------------------------------------------

.. code-block:: console

   $ sudo dnf install -y epel-release || sudo dnf install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1).noarch.rpm
   $ sudo dnf install -y https://apache.jfrog.io/artifactory/arrow/almalinux/$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)/apache-arrow-release-latest.rpm
   $ sudo dnf config-manager --set-enabled epel || :
   $ sudo dnf config-manager --set-enabled powertools || :
   $ sudo dnf config-manager --set-enabled codeready-builder-for-rhel-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)-rhui-rpms || :
   $ sudo subscription-manager repos --enable codeready-builder-for-rhel-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)-$(arch)-rpms || :
   $ sudo dnf install -y datafusion-devel # For C
   $ sudo dnf install -y datafusion-glib-devel # For GLib

AlmaLinux 9 and Red Hat Enterprise Linux 9
------------------------------------------

.. code-block:: console

   $ sudo dnf install -y epel-release || sudo dnf install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1).noarch.rpm
   $ sudo dnf install -y https://apache.jfrog.io/artifactory/arrow/almalinux/$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)/apache-arrow-release-latest.rpm
   $ sudo dnf config-manager --set-enabled epel || :
   $ sudo dnf config-manager --set-enabled crb || :
   $ sudo dnf config-manager --set-enabled codeready-builder-for-rhel-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)-rhui-rpms || :
   $ sudo subscription-manager repos --enable codeready-builder-for-rhel-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)-$(arch)-rpms || :
   $ sudo dnf install -y datafusion-devel # For C
   $ sudo dnf install -y datafusion-glib-devel # For GLib

Build from source
-----------------

If you don't need GLib API, you can use `Cargo <cargo_>`__ based
install instruction:

.. code-block:: console

   $ cargo install cargo-c
   $ cargo cbuild
   $ cargo cinstall

See `the cargo-c's document <cargo-c_>`__ for more build and install
options.

If you need GLib API, you need both of `Cargo <cargo_>`_ and
`Meson <meson_>`_.

.. code-block:: console

   $ cargo install cargo-c
   $ meson setup build .
   $ meson install -C build

See `the Meson's Built-in options document
<meson-built-in-options_>`__ for more setup options. Especially, you
need to know the ``--prefix`` option.

Here are DataFusion GLib specific options:

.. list-table:: DataFusion GLib specific options
   :header-rows: 1

   * - Syntax
     - Description
     - Default
   * - ``-Ddoc=BOOLEAN``
     - Whether document is built by `GTK-Doc <gtk-doc_>`__ or not.
     - ``false``
   * - ``-Dvapi=BOOLEAN``
     - Whether `Vala <vala_>`__ support is built by or not.
     - ``false``

.. _cargo-c: https://github.com/lu-zero/cargo-c
.. _cargo: https://doc.rust-lang.org/cargo/
.. _gtk-doc: https://developer-old.gnome.org/gtk-doc-manual/stable/
.. _meson-built-in-options: https://mesonbuild.com/Builtin-options.html
.. _meson: https://mesonbuild.com/
.. _vala: https://vala.dev/
