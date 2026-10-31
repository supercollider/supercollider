// frames
// In legacy code, a function's arguments and leading variables are all in its frame from the start, in the same order.
FrameLegacy {
    // expect: no diagnostics
    method { |a, b = 2| var x, y = 3; ^{ |c| var z; x = a + c } }
    // expect: no diagnostics
    nested_return { |x| if (x) { ^1 }; ^2 }
}
