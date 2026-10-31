//# VERSION 3.16
// A method can't share a name with another of its class's methods, or with one of its accessors, as in the old
// compiler. The first is kept.
DuplicateMethods {
    var <getter, <>both, >setter, plain;
    classvar <classGetter, <>classBoth;
    const <constant = 1;

    twice { ^1 }
    twice { ^2 }
    *twiceClass { ^1 }
    *twiceClass { ^2 }

    getter { ^1 }
    both { ^1 }
    both_ { ^1 }
    setter_ { ^1 }
    *classGetter { ^1 }
    *classBoth_ { ^1 }
    *constant { ^1 }

    // Members without that accessor are fine, as are methods of the other class.
    // expect: no diagnostics
    plain { ^1 }
    // expect: no diagnostics
    setter { ^1 }
    // expect: no diagnostics
    classGetter { ^1 }
    // expect: no diagnostics
    *getter { ^1 }
}

// An extension can replace an accessor, it is used instead.
+ DuplicateMethods {
    // expect: no diagnostics
    plain_ { ^1 }
}
