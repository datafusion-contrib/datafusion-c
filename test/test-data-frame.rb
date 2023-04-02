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

class DataFrameTest < Test::Unit::TestCase
  def setup
    context = DataFusion::SessionContext.new
    table = Arrow::Table.new(number: [1, 2, 3])
    context.register_table("data", table)
    @data_frame = context.sql("SELECT * FROM data")
    Dir.mktmpdir do |tmp_dir|
      @tmp_dir = tmp_dir
      yield
    end
  end

  def test_to_table
    assert_equal(Arrow::Table.new(number: Arrow::UInt8Array.new([1, 2, 3])),
                 @data_frame.to_table)
  end

  sub_test_case("#write_parquet") do
    def test_no_properties
      path = File.join(@tmp_dir, "parquet")
      @data_frame.write_parquet(path)
      assert_equal(@data_frame.to_table,
                   Arrow::Table.load(File.join(path, "part-0.parquet")))
    end

    def test_max_row_group_size
      path = File.join(@tmp_dir, "parquet")
      properties = DataFusion::ParquetWriterProperties.new
      properties.max_row_group_size = 1
      @data_frame.write_parquet(path, properties)
      parquet_path = File.join(path, "part-0.parquet")
      Arrow::MemoryMappedInputStream.open(parquet_path) do |input|
        reader = Parquet::ArrowFileReader.new(input)
        assert_equal(3, reader.n_row_groups)
      end
    end
  end
end
