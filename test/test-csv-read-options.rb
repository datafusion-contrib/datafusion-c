# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

class CSVReadOptionsTest < Test::Unit::TestCase
  def setup
    @options = DataFusion::CSVReadOptions.new
  end

  def test_has_header
    assert do
      @options.has_header?
    end
    @options.has_header = false
    assert do
      not @options.has_header?
    end
  end

  def test_delimiter
    assert_equal(",".ord, @options.delimiter)
    @options.delimiter = "\t".ord
    assert_equal("\t".ord, @options.delimiter)
  end

  def test_schema
    assert_nil(@options.schema)
    schema = Arrow::Schema.new("a" => :int64)
    @options.schema = schema
    assert_equal(schema, @options.schema)
  end

  def test_schema_infer_max_n_records
    assert_equal(1000, @options.schema_infer_max_n_records)
    @options.schema_infer_max_n_records = 100
    assert_equal(100, @options.schema_infer_max_n_records)
  end

  def test_file_extension
    assert_equal(".csv", @options.file_extension)
    @options.file_extension = ".tsv"
    assert_equal(".tsv", @options.file_extension)
  end

  def test_table_partition_columns
    assert_nil(@options.table_partition_columns)
    @options.table_partition_columns = ["a", "b"]
    assert_equal(["a", "b"], @options.table_partition_columns)
  end
end
