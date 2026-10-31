// compiled
// A bracketed literal default or initial value is computed when the slot is nil, but the prototype frame holds the
// literal too, as the old compiler had it. A class name is read, and other expressions are computed, so they have none.
BracketedPrototypes {
    // expect: no diagnostics
    literals { |a = (false), b(""), c(#[]), d(#[1, 2]), e(-1), f(\sym)| var g = (3), h = ("x"); ^[a, b, c, d, e, f, g, h] }
    // expect: no diagnostics
    computed { |a(Object), b = ([1]), c = (1 + 2)| var d = (a.class); ^[a, b, c, d] }
    // expect: no diagnostics
    function { ^{ |x(4), y(Object)| var z = (#[5]); [x, y, z] } }
}
