<!--
  Copyright 2023 Sutou Kouhei <kou@clear-code.com>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
-->

# Release

## How to release

Create an issue for new release such as "Prepare X.Y.Z". See
https://github.com/datafusion-contrib/datafusion-c/issues/49 as an
example.

Prepare new release:

```bash
git switch -c prepare-X.Y.Z
dev/release/prepare.sh X.Y.Z
editor doc/source/news/X.Y.Z.md
git add doc/source/news/X.Y.Z.md
git commit
git push origin prepare-X.Y.Z
```

Open a pull request from the `prepare-X.Y.Z` branch.

Merge the pull request.

Tag `X.Y.Z`:

```bash
git clone git@github.com:datafusion-contrib/datafusion-c.git
cd datafusion-c
dev/release/tag.sh
```

Wait about 30 minutes for all artifacts are built and uploaded to a
release page. For example,
https://github.com/datafusion-contrib/datafusion-c/releases/tag/21.0.0
is the release page for 21.0.0.

Publish packages for Debian, Ubuntu and AlmaLinux:

```bash
cd package
rake apt:rc
rake yum:rc
rake apt:release
rake yum:release
```
