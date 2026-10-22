// The Interpreter's members, a to z and the rest, are variables of interpreted code.
// expect: no diagnostics
// expect-reads: a -> a
// expect-reads: x -> x
// expect-reads: cmdLine -> cmdLine
a = 1; x.postln; a + x; cmdLine
