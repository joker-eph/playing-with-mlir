
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

"my.op_with_a_loop_cfg"() ({
  // We will loop from 1:10
  %constant_1 = "my.constant"() { value = 1 : i32 } : () -> i32
  %constant_10 = "my.constant"() { value = 10 : i32 } : () -> i32
  "my.branch"() [^loop] : () -> ()

^loop(%loop_index : i32):
  %next_iteration_index = "my.add"(%loop_index, %constant_1) : (i32, i32) -> i32
  %is_terminated = "my.lower_than"(%next_iteration_index, %constant_10) : (i32, i32) -> i1
  "my.branch"(%is_terminated, %next_iteration_index) [^loop, ^exit] : (i1, i32) -> ()

^exit():
  // A block in a CFG cannot be empty, there must be a terminator.
  "my.block_terminator"() : () -> ()

}) : () -> (i32, f64)





