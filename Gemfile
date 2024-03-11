# -*- ruby -*-
#
# Copyright 2022-2024 Sutou Kouhei <kou@clear-code.com>
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

source "https://rubygems.org/"

# Use the version of red-arrow based on the available arrow-glib version
red_arrow_version = ">= 0"
IO.pipe do |input, output|
  begin
    pid = spawn("pkg-config", "--modversion", "arrow-glib",
                out: output,
                err: File::NULL)
    output.close
    _, status = Process.waitpid2(pid)
    if status.success?
      arrow_glib_version = input.read.strip.sub(/-SNAPSHOT\z/, "").strip
      red_arrow_version = "<= #{arrow_glib_version}"
    end
  rescue SystemCallError
  end
end

gem "rake"
gem "red-arrow", red_arrow_version
gem "red-parquet", red_arrow_version
gem "test-unit"
