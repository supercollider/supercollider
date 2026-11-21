// Only the first line sets the file's version, so a version comment below it is misplaced, and the code is compiled
// as legacy, the default for code without one,
// where a variable can't be declared after an expression.
// expect-warning: Misplaced version
// expect-error: Variable declared after an expression
//# VERSION 3.16
(var a = 1; a; var b = 2; b)
