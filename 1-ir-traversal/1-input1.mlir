
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

%add = "my.add"(%a, %b) : (i32, i32) -> i32
%sub = "my.sub"(%add, %b) : (i32, i32) -> i32


