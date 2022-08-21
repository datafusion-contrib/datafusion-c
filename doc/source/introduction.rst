.. Copyright 2022 Sutou Kouhei <kou@clear-code.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and

Introduction
============

DataFusion C provides C API for `DataFusion <datafusion_>`_ that is an
extensible query execution framework written in Rust. C API is useful
to use DataFusion from other languages.

APIs
----

DataFusion C provides 2 APIs:

* Raw C API
* `GLib <glib_>`__ API built on top of raw C API

If you already use GLib, GLib API is better. If you don't use GLib,
raw C API is enough.

.. _datafusion: https://arrow.apache.org/datafusion/
.. _glib: https://docs.gtk.org/glib/
