// interpreted-version: 3.16
// In interpreted code, a closed function sees the Interpreter's members, but not the region's variables.
// expect-error: Variable outside closed function
// expect-reads: a -> a
(var foo = 1; #{ a + foo })
