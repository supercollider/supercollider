// compiled
// The prototype frame holds a literal default or initial value, any other is computed when the function runs, so its
// slot has no prototype. A method's slots are `this`, its arguments, its variadic arguments and its variables.
Prototypes {
    // expect: no diagnostics
    literals { |a = 1, b = \sym, c = "text", d = #[1, 2]| var e = 2.5, f = $c, g; ^[a, b, c, d, e, f, g] }
    // expect: no diagnostics
    computed { |a = ([1, 2]), b = (1 + 2)| var c = [a], d = b.neg; ^[c, d] }
    // expect: no diagnostics
    variadic { |a ...rest, kwargs| var b = 1; ^[a, rest, kwargs, b] }
    // expect: no diagnostics
    function { ^{ |x = 3, y| var z = 4, w = x; [y, z, w] } }
}
