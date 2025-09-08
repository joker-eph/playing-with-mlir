
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

"my.op_with_multiple_regions"() ({
  %add = "my.add"(%a, %b) : (i32, i32) -> i32
  %sub = "my.sub"(%add, %b) : (i32, i32) -> i32
}, {
  %add = "my.add2"(%a, %b) : (i32, i32) -> i32
  %sub = "my.sub3"(%add, %b) : (i32, i32) -> i32
}, {
  // Empty regions
}) : () -> (i32, f64)



