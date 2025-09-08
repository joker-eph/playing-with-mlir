
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

%result1, %result2 = "my.op_with_multiple_results"() : () -> (i32, f64)


%add = "my.add"(%a, %b) : (i32, i32) -> i32
%sub = "my.sub"(%add, %b) : (i32, i32) -> i32



"my.op_with_attr"() { some_key = "string value", integer_attr = 42 : i32, int_array = array<i64: 1, 2, 3>} : () -> ()


"my.op_with_properties"() <{ some_key = "string value", integer_attr = 42 : i32, int_array = array<i64: 1, 2, 3>}> : () -> ()
