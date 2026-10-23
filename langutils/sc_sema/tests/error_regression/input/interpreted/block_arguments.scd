// interpreted-version: 3.16
// Arguments of `( ... )` are their defaults, they hide the Interpreter's members.
// expect: no diagnostics
// expect-reads: w -> w = 3
// expect: no result
(|w = 3| w)
