// bytecode
//# VERSION 3.16
// Each getter and setter is a shortcut method of the member's class, a class variable's or constant's of the meta class.
// A constant without a value returns nil. An extension's method replaces an accessor. Inherited members' accessors are
// their own class's. As Legacy's, see versions/bytecode_accessors.
AccessorsBase316 {
    var <inherited;
}

Accessors316 : AccessorsBase316 {
    var <getter, <>both, >setter, plain;
    classvar <classGetter, <>classBoth, classPlain;
    const <constant = 1, <array = #[1, 2], <valueless, hidden = 2;
}

+ Accessors316 {
    // expect: no diagnostics
    both_ { |value| both = value + 1 }
    // expect: no diagnostics
    *classGetter { ^nil }
}
