// Legacy interpreted code, a region's variables are also declared at its start.
// expect-error: Variable declared after an expression
(var a = 1; a; var b = 2; b)
