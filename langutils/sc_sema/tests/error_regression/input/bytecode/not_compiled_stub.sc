// lir
// bytecode
//# VERSION 3.16
// A method the backend can't lower is followed by one that throws PoisonedMethodError(thisMethod), as a poisoned method
// is, when the library defines it, whatever it is called with. Its failed functions are left in the lowered ir.
PoisonedMethodError {
    *new { |method| ^super.new }
    throw { }
}

NotCompiledStub {
    foo { |... a| ^a }
    // expect-error: Not compiled
    two_variadic { |a, b| ^this.foo(*a, *b) }
    // A nested function that can't be lowered fails its method.
    // expect-error: Not compiled
    in_function { |a, b| ^{ this.foo(*a, *b) } }
    // expect: no diagnostics
    fine { ^1 }
}
