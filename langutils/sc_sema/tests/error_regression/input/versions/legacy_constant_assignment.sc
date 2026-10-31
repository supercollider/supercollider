// The old compiler reported this too, "Assigning to a constant."
// A constant can't be assigned, in a method, a class method, a pack or a function.
LegacyConstantAssignment {
    const a = 1, <b = 2;
    // A constant can only have a getter, the setter is left out.
    const >c = 3, <>d = 4;
    classvar e;

    // expect-error: Assigning to a constant
    instance_method { a = 2; ^a }
    // expect-error: Assigning to a constant
    *class_method { a = 2; ^a }
    // expect-error: Assigning to a constant
    pack { #a, e = [1, 2]; ^e }
    // expect-error: Assigning to a constant
    in_function { ^{ b = 3 } }
    // expect-error: Assigning to a constant
    nested_return { |x| if (x) { a = 2; ^1 }; ^2 }

    // expect: no diagnostics
    reads { ^a + b }
    // expect: no diagnostics
    class_variable { e = 1; ^e }
    // A local hides the constant.
    // expect: no diagnostics
    hidden { var a = 1; a = 2; ^a }
}
