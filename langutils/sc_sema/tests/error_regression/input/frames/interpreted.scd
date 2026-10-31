// interpreted-version: 3.16
// frames
// Each region is its own function, whose frame has no parent. The Interpreter's members, e.g., `a`, are instance slots.
// expect: no diagnostics
( var x = 1; a = x );
( var y = 2; { |z| y + z + a } )
