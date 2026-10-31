// lir
//# VERSION 3.16
// Each function is its own lir function, pushed where it is written. A function returns its last value, or nil, a
// method returns this unless it ends with `^`. Only a function's last send, or one after `^`, is a tail call.
LirFunctions {
    // expect: no diagnostics
    returns_this { |a| a.foo }
    // expect: no diagnostics
    nested { |a| ^{ |b| var c = a + b; { c.bar } } }
    // expect: no diagnostics
    empty_function { ^{} }
    // expect: no diagnostics
    function_ending_in_declaration { ^{ var x = 1 } }
    // expect: no diagnostics
    nested_return { |x| if (x) { ^1 }; ^2 }
    // A literal default is in the frame, any other runs when the argument is nil.
    // expect: no diagnostics
    defaults { |a = 1, b = (a + 1), c| ^[a, b, c] }
    // expect: no diagnostics
    partial_application { ^_.foo(1) }
}
