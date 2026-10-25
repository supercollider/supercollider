// bytecode
//# VERSION 3.16
// As Legacy's (versions/bytecode_shortcuts), and a method that only assigns its first argument to an instance variable
// is a shortcut too. An immutable constant is pushed as a literal, so it is returned as one.
BytecodeShortcuts316 {
    const kConst = 1;
    var inst, other;
    classvar cv;

    // expect: no diagnostics
    assign { |a| inst = a }
    // expect: no diagnostics
    assignTwo { |a, b| other = a }
    // expect: no diagnostics
    returnConstant { ^kConst }
    // expect: no diagnostics
    returnArgument { |a| ^a }
    // expect: no diagnostics
    redirect { |a| ^this.foo(a) }
    // The local is never assigned, so is known to be nil.
    // expect: no diagnostics
    hiddenReturn { var other; ^other }

    // Not shortcuts.
    // expect: no diagnostics
    assignLiteral { inst = 5 }
    // expect: no diagnostics
    assignSecond { |a, b| inst = b }
    // expect: no diagnostics
    assignReturned { |a| ^inst = a }
    // expect: no diagnostics
    assignClassVariable { |a| cv = a }
    // expect: no diagnostics
    hiddenAssign { |a| var inst; inst = a }
    // expect: no diagnostics
    nonLocal { |a| if (a) { ^1 }; ^2 }
}
