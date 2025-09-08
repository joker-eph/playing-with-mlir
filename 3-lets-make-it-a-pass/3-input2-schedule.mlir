// Let's understand a pass pipeline scheduling.
// Here is a baseline invocation
// ./a.out  3-input2-schedule.mlir -allow-unregistered-dialect  -o /dev/null \
//  --pass-pipeline="builtin.module(my-pass)"
//
// We can now try to play with the pipeline specification. The IR below has a
// few "func.func" operations at the top-level (inside the implicit
// "builtin.module"), we can schedule the pass to be running on these instead:
//
// ./a.out  3-input2-schedule.mlir -allow-unregistered-dialect  -o /dev/null \
//  --pass-pipeline="builtin.module(func.func(my-pass))"
//
// The output is likely weirdly intertwined, this is because MLIR will
// automatically map the nesting in the pass pipeline specification to
// different threads. Let's disabled this with --mlir-disable-threading
//
// ./a.out  3-input2-schedule.mlir -allow-unregistered-dialect  -o /dev/null \
//  --mlir-disable-threading \
//  --pass-pipeline="builtin.module(func.func(my-pass))"



// This is a declaration, the function body is empty.
func.func private @foo()

// A function with a body
func.func @bar() {
  %a = "my.input1"() : () -> i32
  %b = "my.input2"() : () -> i32
  return
}

// Another function with a body
func.func @baz() {
  %a = "my.input3"() : () -> i32
  %b = "my.input4"() : () -> i32
  return
}


