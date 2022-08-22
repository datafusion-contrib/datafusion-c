.. Copyright 2022 Sutou Kouhei <kou@clear-code.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and

Example - SQL
=============

This section shows examples that execute a simple SQL in
multiple ways.

.. _example-sql-raw-c-api-from-c:

Raw C API from C
----------------

The following code shows how to execute a simple SQL with raw C API
in C. Here are points:

* You can create a session context (:c:struct:`DFSessionContext`) by
  :c:func:`df_session_context_new()`.
* You can run a SQL by :c:func:`df_session_context_sql()`.
* You can get details on error by :c:struct:`DFError`. See also
  :c:func:`df_error_get_code()` and :c:func:`df_error_get_message()`.
* You need to free :c:struct:`DFError` by :c:func:`df_error_free()`
  when no longer needed.
* You can get :c:struct:`DFDataFrame` as a result of the SQL execution.
* You can show :c:struct:`DFDataFrame` contents by
  :c:func:`df_data_frame_show()`.
* You can free :c:struct:`DFDataFrame` by
  :c:func:`df_data_frame_free()`.
* You can free :c:struct:`DFSessionContext` by
  :c:func:`df_session_context_free()`.

.. literalinclude:: ../../../example/sql.c
   :language: c
   :linenos:

.. _example-sql-glib-api-from-c:

GLib API from C
---------------

The following code shows how to execute a simple SQL with GLib API
in C. Here are points:

* You can create a session context (`GDFSessionContext`_) by
  `gdf_session_context_new()`_.
* You can run a SQL by `gdf_session_context_sql()`_.
* You can handle error the usual way in GLib. See `GError`_.
* You can get `GDFDataFrame`_ as a result of the SQL execution.
* You can show `GDFDataFrame`_ contents by
  `gdf_data_frame_show()`_.
* You can free all `GObject`_ based objects such as
  `GDFSessionContext`_ and `GDFDataFrame`_ by `g_object_unref()`_.

.. literalinclude:: ../../../example/sql-glib.c
   :language: c
   :linenos:

.. _example-sql-raw-c-api-from-python:

Raw C API from Python
---------------------

The following code shows how to execute a simple SQL with raw C
API in Python. You can use a FFI library such as `ctypes
<ctypes_>`__ to use raw C API from Python. Here are points:

* You need to open DataFusion C's shared library explicitly but shared
  library name depends on platform. For example, it's
  ``libdatafusion.so`` on Linux. ``ctypes.util.find_library()`` may help you.
* You need to open DataFusion C's shared library by ``ctypes.CDLL()``.
* You need to prepare signature of raw C API's functions that you need
  to use before you call them.
* See :ref:`example-sql-raw-c-api-from-c` how to use raw C API.

.. literalinclude:: ../../../example/sql.py
   :language: python
   :linenos:

.. _example-sql-glib-api-from-python:

GLib API from Python
--------------------

The following code shows how to execute a simple SQL with GLib API in
Python. You can use `PyGObject`_ to use GLib API from Python. Here are
points:

* You can use ``DataFusion`` as GObject Introspection namespace.
* See :ref:`example-sql-glib-api-from-c` how to use GLib API.

.. literalinclude:: ../../../example/sql-glib.py
   :language: python
   :linenos:

.. _example-sql-raw-c-api-from-ruby:

Raw C API from Ruby
-------------------

The following code shows how to execute a simple SQL with raw C
API in Ruby. You can use a FFI library such as `Fiddle
<fiddle_>`__ to use raw C API from Ruby. Here are points:

* You need to open DataFusion C's shared library explicitly but shared
  library name depends on platform. For example, it's
  ``libdatafusion.so`` on Linux. ``RbConfig::CONFIG["SOEXT"]`` may
  help you.
* You need to open DataFusion C's shared library by
  ``Fiddle::Importer.dlload``.
* You need to register raw C API's functions that you need
  to use explicitly before you call them.
* See :ref:`example-sql-raw-c-api-from-c` how to use raw C API.

.. literalinclude:: ../../../example/sql.rb
   :language: ruby
   :linenos:

.. _example-sql-glib-api-from-ruby:

GLib API from Ruby
------------------

The following code shows how to execute a simple SQL with GLib API in
Ruby. You can use `gobject-introspection gem
<gobject-introspection-gem_>`__ or `red-datafusion gem
<red-datafusion-gem_>`__ to use GLib API from Ruby. The following
example uses gobject-introspection gem but you should use
red-datafusion gem in production. red-datafusion gem is built on top
of the gobject-introspection gem and adds many useful APIs. Here are
points:

* You can use ``DataFusion`` as GObject Introspection namespace.
* See :ref:`example-sql-glib-api-from-c` how to use GLib API.

.. literalinclude:: ../../../example/sql-glib.rb
   :language: ruby
   :linenos:

GLib API from Vala
------------------

The following code shows how to execute a simple SQL with GLib API in
Vala. You need to add ``-Dvapi=true`` when you run ``meson setup`` to
enable Vala support. Here are points:

* You can use ``datafusion-glib`` as Vala package name.
* You can get a directory where Vala API file for ``datafusion-glib``
  package exists by ``pkg-config --variable=vapidir datafusion-glib``.
* See :ref:`example-sql-glib-api-from-c` how to use GLib API.

.. literalinclude:: ../../../example/sql-glib.vala
   :language: vala
   :linenos:

.. _GDFDataFrame: ../glib/class.DataFrame.html
.. _GDFSessionContext: ../glib/class.SessionContext.html
.. _GError: https://docs.gtk.org/glib/struct.Error.html
.. _GObject: https://docs.gtk.org/gobject/class.Object.html
.. _PyGObject: https://pygobject.readthedocs.io/en/latest/
.. _ctypes: https://docs.python.org/3/library/ctypes.html
.. _fiddle: https://github.com/ruby/fiddle
.. _g_object_unref(): https://docs.gtk.org/gobject/method.Object.unref.html
.. _gdf_data_frame_show(): ../glib/method.DataFrame.show.html
.. _gdf_session_context_new(): ../glib/ctor.SessionContext.new.html
.. _gdf_session_context_sql(): ../glib/method.SessionContext.sql.html
.. _gobject-introspection-gem: https://rubygems.org/gems/gobject-introspection
.. _red-datafusion-gem: https://rubygems.org/gems/red-datafusion
