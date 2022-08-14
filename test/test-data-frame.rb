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

class DataFrameTest < Test::Unit::TestCase
  def setup
    context = DataFusion::SessionContext.new
    @data_frame = context.sql("SELECT 1")
  end

  def test_to_table
    schema = Arrow::Schema.new([Arrow::Field.new("Int64(1)", :int64, false)])
    assert_equal(Arrow::Table.new(schema, [Arrow::Int64Array.new([1])]),
                 @data_frame.to_table)
  end
end
