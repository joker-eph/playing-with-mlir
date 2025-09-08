// ./run  3-lets-make-it-a-pass/3-lets-make-it-a-pass.cpp 3-lets-make-it-a-pass/3-input1.mlir
// Try running with command line arguments:
// First with:  <nothing>
// This should fail, but hint at using this option:  -allow-unregistered-dialect
// Now you should see the round-trip of the IR
// Let's try:  --help
// There is a lot of options available! Skim through, feel free to ask about
// what would be unexpected.
// We can run our pass, let's try to add: --pass-pipeline="builtin.module(my-pass)" -o /dev/null
// You can run the pass multiple times actually:  --pass-pipeline="builtin.module(my-pass, my-pass, my-pass)" -o /dev/null
//
// Let's play with the debug mode, add to the command line: -debug
// Now we see the debug output, with indications of source line number.
//
// There is also a type and a level printed, we can filter to only print
// some debug types, try to remove -debug and use instead: --debug-only mypass
//
// The higher the level, the more verbose it is, we can filter out any
// logging from my pass that is higher than two:
// --debug-only mypass:2

%a = "my.input"() : () -> i32
%b = "my.input"() : () -> i32

%add = "my.add"(%a, %b) : (i32, i32) -> i32
%sub = "my.sub"(%add, %b) : (i32, i32) -> i32
