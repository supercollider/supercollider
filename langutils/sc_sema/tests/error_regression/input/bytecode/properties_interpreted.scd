// interpreted-version: 3.16
// bytecode
// Each region's function is explicitly closed, as the old compiler made interpreted code, and the Interpreter's
// members don't stop a function in it being closed.
// expect: no diagnostics
( var x = 1; { a + 1 } );
( var y = a; { y } );
( { this } )
