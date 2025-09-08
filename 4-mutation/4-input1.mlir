
// Verify that the op is "flagged" (and capture its result).
// CHECK: %[[A:.*]] = "my.input"() {my.flagged} : () -> i32
%a = "my.input"() {my.flag}: () -> i32

// Verify that the op is left alone (and capture its result).
// CHECK: %[[B:.*]] = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

// Verify that the op is replaced.
// CHECK: %[[ADD:.*]] = "my.super_add"(%[[A]], %[[B]]) : (i32, i32) -> i32
%add = "my.add"(%a, %b) : (i32, i32) -> i32

// Verify that the operands are reversed
// CHECK: %[[SUB:.*]] = "my.sub"(%[[B]], %[[ADD]]) : (i32, i32) -> i32
%sub = "my.sub"(%add, %b) : (i32, i32) -> i32

// Verify that this op that isn't used will be removed
// CHECK-NOT: my.dead_op
%dead = "my.dead_op"(%sub) : (i32) -> i32
