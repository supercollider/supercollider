//# VERSION 3.16
// The version comment on the first line is the file's, so it is compiled as 3.16, not as legacy, the default for
// code without one.
// A variable declared after an expression is only allowed from 3.16.
// expect: -> 2
// expect: no diagnostics
(var a = 1; a; var b = 2; b)
