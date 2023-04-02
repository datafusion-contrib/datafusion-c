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

class ParquetReadOptionsTest < Test::Unit::TestCase
  def setup
    @options = DataFusion::ParquetReadOptions.new
  end

  def test_file_extension
    assert_equal(".parquet", @options.file_extension)
    @options.file_extension = ".pqt"
    assert_equal(".pqt", @options.file_extension)
  end

  def test_table_partition_columns
    assert_nil(@options.table_partition_columns)
    table_partition_columns = Arrow::Schema.new(a: :int8,
                                                b: :boolean)
    @options.table_partition_columns = table_partition_columns
    assert_equal(table_partition_columns,
                 @options.table_partition_columns)
  end

  def test_pruning
    assert do
      not @options.set_pruning?
    end
    @options.pruning = false
    assert do
      @options.set_pruning?
    end
    assert do
      not @options.pruning?
    end
    @options.unset_pruning
    assert do
      not @options.set_pruning?
    end
  end
end
