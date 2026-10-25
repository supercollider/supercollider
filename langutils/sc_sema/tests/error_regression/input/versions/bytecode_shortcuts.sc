// bytecode
// A method that only returns `this`, a literal, an argument or an instance variable, or sends to `this`, `super` or a
// member with all its arguments in order, is a shortcut the VM runs without code. Checked against old sclang, except
// where marked as fixed. See bytecode/shortcuts for 3.16.
BytecodeShortcuts {
    const kConst = 1;
    var inst, other;
    classvar cv;

    // expect: no diagnostics
    empty {}
    // expect: no diagnostics
    returnThis { ^this }
    // expect: no diagnostics
    returnInteger { ^1 }
    // expect: no diagnostics
    returnNil { ^nil }
    // expect: no diagnostics
    returnString { ^"text" }
    // expect: no diagnostics
    returnSymbol { ^\name }
    // expect: no diagnostics
    returnArray { ^#[1, 2] }
    // expect: no diagnostics
    returnArgument { |a, b| ^b }
    // expect: no diagnostics
    returnLiteralArgument { |a = 1| ^a }
    // expect: no diagnostics
    returnInstance { ^other }
    // expect: no diagnostics
    redirect { |a, b| ^this.foo(a, b) }
    // expect: no diagnostics
    redirectCall { |a| ^foo(this, a) }
    // expect: no diagnostics
    redirectSuper { ^super.foo }
    // expect: no diagnostics
    forwardInstance { |a| ^other.foo(a) }
    // expect: no diagnostics
    forwardClassVariable { ^cv.foo }

    // Not shortcuts. The old compiler checked for assigning to an instance variable, but its parse tree never matched.
    // expect: no diagnostics
    assign { |a| inst = a }
    // expect: no diagnostics
    implicitReturn { 1 }
    // expect: no diagnostics
    returnClass { ^Object }
    // expect: no diagnostics
    returnFunction { ^{ 1 } }
    // expect: no diagnostics
    returnConstant { ^kConst }
    // expect: no diagnostics
    returnClassVariable { ^cv }
    // expect: no diagnostics
    returnComputed { ^(1 + 2) }
    // expect: no diagnostics
    computedDefault { |a = (1)| ^a }
    // expect: no diagnostics
    computedVariable { var a = (1); ^1 }
    // expect: no diagnostics
    variadic { |...a| ^1 }
    // expect: no diagnostics
    primitive { _ObjectClass ^1 }
    // expect: no diagnostics
    doesNotUnderstand { ^1 }
    // expect: no diagnostics
    assignThenReturn { |a| inst = a; ^this }
    // expect: no diagnostics
    redirectWrongOrder { |a, b| ^this.foo(b, a) }
    // expect: no diagnostics
    redirectMissing { |a, b| ^this.foo(a) }
    // expect: no diagnostics
    redirectKeyword { |a| ^this.foo(a: a) }
    // expect: no diagnostics
    redirectBinary { |a| ^this + a }
    // expect: no diagnostics
    redirectSetter { |a| ^this.foo = a }
    // expect: no diagnostics
    redirectVariadic { |a| ^this.foo(*a) }
    // expect: no diagnostics
    forwardArgument { |a| ^a.foo }
    // expect: no diagnostics
    nonLocal { |a| if (a) { ^1 }; ^2 }

    // Fixed: the old compiler returned the instance variable the local hides.
    // expect: no diagnostics
    hiddenReturn { var other; ^other }
}
