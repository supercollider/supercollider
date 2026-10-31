// interpreted-version: 3.16
// Partial application and generators use the same compiler as class library code.
// expect: no diagnostics
a = _ + 1; b = {: x, x <- [1, 2] }; c = { |n| n * a.(n) }; c.(2)
