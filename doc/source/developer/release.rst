.. Copyright 2022 Sutou Kouhei <kou@clear-code.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and

Release
=======

This document describes how to release a new version.

Prepare
-------

1. Run ``dev/release/prepare.sh`` to create a branch for a new version
   and prepare the branch.
2. Update ``doc/source/news/${VERSION}.md``.
3. Commit changes.
4. Push the commit.
5. Open a pull request.

Here are command lines for them:

.. code-block:: console

   $ dev/release/prepare.sh ${VERSION}
   $ editor doc/source/news/${VERSION}.md
   $ git add doc/source/news/${VERSION}.md
   $ git commit
   $ git push

Create a pull request for the branch on
https://github.com/datafusion-contrib/datafusion-c/pulls .

Tag and upload
--------------

We can tag after we merge the prepare branch:

.. code-block:: console

   $ dev/release/tag.sh

Our CI jobs create ``.deb`` / ``.rpm`` packages automatically when we
tag a new version. It takes about 20min. We need to wait for finishing
these jobs.

We can download the built ``.deb`` / ``.rpm`` packages and upload to
https://apache.jfrog.io/artifactory/arrow/ after these jobs are
finished. Note that this must be done by an Apache Arrow PMC member
who registers his/her GPG key to
https://dist.apache.org/repos/dist/release/arrow/KEYS . Because
APT/Yum repositories at https://apache.jfrog.io/artifactory/arrow/ are
for Apache Arrow. We just reuse these APT/Yum repositories. So we must
follow policy of https://apache.jfrog.io/artifactory/arrow/ .

.. see-also::

   `Release Management Guide for Apache Arrow <apache-arrow-release_>`__.

.. code-block:: console

   $ rake -C package apt:rc yum:rc
   $ dev/package/verify-packages.sh rc
   $ rake -C package apt:release yum:release
   $ dev/package/verify-packages.sh release

.. _apache-arrow-release: https://arrow.apache.org/docs/developers/release.html
