#!/usr/bin/env ruby
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

$VERBOSE = true

require "pathname"

(ENV["DATAFUSION_DLL_PATH"] || "").split(File::PATH_SEPARATOR).each do |path|
  RubyInstaller::Runtime.add_dll_directory(path)
end

source_dir = Pathname.new(__dir__).parent.expand_path
lib_dir = source_dir + "lib"
test_dir = source_dir + "test"

$LOAD_PATH.unshift(lib_dir.to_s)

require_relative "helper"

exit(Test::Unit::AutoRunner.run(true, test_dir.to_s))
