// bytecode
// Each getter and setter is a shortcut method of the member's class, a class variable's or constant's of the meta class.
// An extension's method replaces an accessor. Checked against old sclang. See bytecode/accessors for 3.16.
AccessorsBase {
    var <inherited;
}

Accessors : AccessorsBase {
    var <getter, <>both, >setter, plain;
    classvar <classGetter, <>classBoth, classPlain;
    const <constant = 1, <array = #[1, 2], hidden = 2;
}

+ Accessors {
    // expect: no diagnostics
    both_ { |value| both = value + 1 }
    // expect: no diagnostics
    *classGetter { ^nil }
}
