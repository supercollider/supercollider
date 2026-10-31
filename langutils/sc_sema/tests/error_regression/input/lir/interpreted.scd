// interpreted-version: 3.16
// lir
// Each region is its own function, the Interpreter's members are instance slots.
// expect: no diagnostics
( var x = 1; a = x + 1 );
( { |y| y + a } )
