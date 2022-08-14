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

class SessionContextTest < Test::Unit::TestCase
  def setup
    @context = DataFusion::SessionContext.new
  end

  def test_sql
    schema = Arrow::Schema.new([Arrow::Field.new("Int64(1)", :int64, false)])
    assert_equal(Arrow::Table.new(schema, [Arrow::Int64Array.new([1])]),
                 @context.sql("SELECT 1").to_table)
  end

  def test_deregister
    record_batch = Arrow::RecordBatch.new(boolean: [true, false, nil],
                                          integer: [1, nil, 3])
    assert do
      @context.register_record_batch("data", record_batch)
    end
    data_frame = @context.sql("SELECT * FROM data")
    assert_equal(record_batch.to_table, data_frame.to_table)
    assert do
      @context.deregister("data")
    end
    message = "[session-context][sql] Error during planning: " +
              "'datafusion.public.data' not found"
    assert_raise(DataFusion::Error::Plan.new(message)) do
      @context.sql("SELECT * FROM data")
    end
  end

  def test_register_record_batch
    record_batch = Arrow::RecordBatch.new(boolean: [true, false, nil],
                                          integer: [1, nil, 3])
    assert do
      @context.register_record_batch("data", record_batch)
    end
    data_frame = @context.sql("SELECT * FROM data")
    assert_equal(record_batch.to_table, data_frame.to_table)
  end

  def test_register_table
    boolean_chunks = [
      Arrow::Array.new([true]),
      Arrow::Array.new([false, nil]),
    ]
    integer_chunks = [
      Arrow::Array.new([1, nil]),
      Arrow::Array.new([3]),
    ]
    table = Arrow::Table.new(boolean: Arrow::ChunkedArray.new(boolean_chunks),
                             integer: Arrow::ChunkedArray.new(integer_chunks))
    @context.register_table("data", table)
    data_frame = @context.sql("SELECT * FROM data")
    assert_equal(table, data_frame.to_table)
  end

  sub_test_case("#register_csv") do
    def setup
      super
      Tempfile.open(["datafusion", ".csv"]) do |csv_file|
        @csv_file = csv_file
        @csv_file.puts(<<-CSV)
a,b,c
1,2,3
10,20,30
        CSV
        @csv_file.close
        schema = Arrow::Schema.new([
                                     Arrow::Field.new("a", :int64, false),
                                     Arrow::Field.new("b", :int64, false),
                                     Arrow::Field.new("c", :int64, false),
                                   ])
        @table = Arrow::Table.new(schema,
                                  [
                                    Arrow::Int64Array.new([1, 10]),
                                    Arrow::Int64Array.new([2, 20]),
                                    Arrow::Int64Array.new([3, 30]),
                                  ])
        yield
      end
    end

    def test_no_options
      assert do
        @context.register_csv("data", @csv_file.path)
      end
      data_frame = @context.sql("SELECT * FROM data")
      assert_equal(@table, data_frame.to_table)
    end

    def test_options
      options = DataFusion::CSVReadOptions.new
      schema = Arrow::Schema.new([
                                   Arrow::Field.new("a", :int8, false),
                                   Arrow::Field.new("b", :int8, false),
                                   Arrow::Field.new("c", :int8, false),
                                 ])
      table = Arrow::Table.new(schema,
                               [
                                 Arrow::Int8Array.new([1, 10]),
                                 Arrow::Int8Array.new([2, 20]),
                                 Arrow::Int8Array.new([3, 30]),
                               ])
      options.schema = schema
      assert do
        @context.register_csv("data", @csv_file.path, options)
      end
      data_frame = @context.sql("SELECT * FROM data")
      assert_equal(table, data_frame.to_table)
    end
  end
end
