// interpreted-version: 3.16
// Variables can be declared after other expressions, and functions can capture them.
// expect: no diagnostics
// expect-reads: c -> c = 1
// expect-reads: f -> f = { c }
var c = 1; c.postln; var f = { c }; a = f; f.()
