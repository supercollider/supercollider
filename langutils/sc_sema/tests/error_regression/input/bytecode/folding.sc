// bytecode
//# VERSION 3.16
// From 3.16, the values sema knows are used, e.g., a variable that is never assigned, which legacy code reads.
BytecodeFolding {
    const kInteger = 2, kString = "s";

    knownVariable { var b = 1; ^b }
    folded { ^(1 + 2) }
    // A new mutable value is a copy of the constant, and of its mutable elements, a literal is shared.
    mutable { ^[1, "a"] }
    literal { ^#[1, "a"] }
    // An immutable class constant is pushed as a literal, a mutable one is the class's object, from the constants table.
    classConstant { ^kInteger }
    mutableClassConstant { ^kString }
    // A variable known to be nil returns with one bytecode, as `^nil` does.
    returnKnownNil { var b; ^b }
    tail { ^{ |a| a.foo } }
}
