<!--
  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an
  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied.  See the License for the
  specific language governing permissions and limitations
  under the License.
-->

# DataFusion C API

C language bindings for DataFusion.

## How to build

### Without GLib API

```bash
cargo build
```

### With GLib API

```bash
meson setup build .
meson install -C build
```

#### Examples

See `examples/sql.c` for C.

```console
$ build/examples/sql-c
+----------+
| Int64(1) |
+----------+
| 1        |
+----------+
```

See `examples/sql.py` for Python.

```console
$ LD_LIBRARY_PATH=$PWD/build examples/sql.py
+----------+
| Int64(1) |
+----------+
| 1        |
+----------+
```

See `examples/sql.rb` for Ruby.

```console
$ LD_LIBRARY_PATH=$PWD/build examples/sql.rb
+----------+
| Int64(1) |
+----------+
| 1        |
+----------+
```

See `examples/sql.vala` for Vala.

You need to add `-Dvala=true` option to `meson setup` to enable Vala
support.

```console
$ build/examples/sql-vala
+----------+
| Int64(1) |
+----------+
| 1        |
+----------+
```
