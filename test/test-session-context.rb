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
    notify("TODO: Use Arrow::Table once ARROW-16931 is released.")
    assert_equal(<<-TABLE, @context.sql("SELECT 1").to_table.to_s)
	Int64(1)
0	       1
                 TABLE
  end

  def test_register_record_batch
    record_batch = Arrow::RecordBatch.new(boolean: [true, false, nil],
                                          integer: [1, nil, 3])
    @context.register_record_batch("data", record_batch)
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
end
