
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

"my.op_with_regions"() ({
  %add = "my.add"(%a, %b) : (i32, i32) -> i32
}) : () -> (i32, f64)

// This operation is using the %add which is not in scope.
%sub = "my.sub"(%add, %b) : (i32, i32) -> i32



