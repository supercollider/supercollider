// Each `( ... )` has its own scope, so its variables are gone after it, and `q` is the Interpreter's again.
// expect: no diagnostics
// expect-reads: q -> q = 1; q
(var q = 1; q); q
