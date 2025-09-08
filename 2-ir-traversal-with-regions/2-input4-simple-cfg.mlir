
%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

"my.op_with_cfg_regions"() ({
  // There is an implicit entry block here, it can't
  // be a successor from other block, you can still optionally
  // name it:
  // ^entry_block:
  %add = "my.add"(%a, %b) : (i32, i32) -> i32
  "my.branch"() [^next_block] : () -> ()

  // Blocks in a CFG can be in any order: try to shuffle them to see.

^next_block:
  // Since the entry block dominates the current block, we can use
  // here the SSA value defined there.
  %sub = "my.sub"(%add, %b) : (i32, i32) -> i32
  // This branch jumps to a block that takes an argument.
  // You can think of a branch as a function call that does
  // not return.
  "my.branch"(%sub) [^a_block_with_arguments] : (i32) -> ()

^a_block_with_arguments(%block_arg : i32):
  // A block in a CFG cannot be empty, there must be a terminator.
  "my.block_terminator"() : () -> ()

}) : () -> (i32, f64)





