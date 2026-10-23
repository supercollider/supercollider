// In legacy code, as in the old compiler, a block's arguments and variables are all in its frame, so every argument
// default and initial value can see all of them. Argument defaults run first, then the variables' initial values, in
// order. A literal initial value is already in the frame before anything runs, anything else is nil until it does.
// 3.16 only allows a name after its declaration.
LegacyLookup {
    // `x1` isn't a literal, so it is nil when `a1`'s default runs, and `a1` is nil.
    // expect-warning: Variable used before its declaration
    // expect-reads: x1 -> x1 =2 + 2
    // expect: not poisoned
    // expect: no result
    argument_sees_computed_variable {
        ^{
            arg a1 = x1;
            var x1 =2 + 2;
            a1
        }.()
    }

    // `x1` is a literal, so it is already in the frame, and `a1` is 4, but `a1` isn't a literal so it isn't known.
    // TODO: check this returns 4 once code can be run.
    // expect-warning: Variable used before its declaration
    // expect-reads: x1 -> x1 = 4
    // expect: not poisoned
    // expect: no result
    argument_sees_literal_variable {
        ^{
            arg a1 = x1;
            var x1 = 4;
            a1
        }.()
    }

    // `f` is nil while its own initial value runs.
    // expect-warning: Variable used before its declaration
    // expect-reads: f -> f = f
    // expect: not poisoned
    // expect: no result
    self_reference { var f = f; ^f }
    // Allowed in 3.16 too, as the function runs after `f` is assigned.
    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    self_reference_in_function { var f = { f }; ^f }
    // expect-warning: Variable used before its declaration
    // expect-reads: b -> b = 1
    // expect: no result
    later_variable { var a = b, b = 1; ^a }
    // expect-warning: Variable used before its declaration
    // expect-reads: b -> b = 1
    later_variable_statement { var a = b; var b = 1; ^a }
    // The frame's `x` hides the outer one before it is declared, where 3.16 would read the outer one.
    // expect-warning: Variable used before its declaration
    // expect-reads: x -> x = 2
    hides_outer { var x = 1; ^{ var a = x, x = 2; a } }
    // expect-warning: Variable used before its declaration
    // expect-reads: g -> g = { 1 }
    later_variable_in_function { var f = { g.() }, g = { 1 }; ^f }
    // expect-warning: Variable used before its declaration
    // expect-reads: b -> b = 1
    later_argument { arg a = b, b = 1; ^a }
    // Assigned by an argument's default before its declaration, so `x` isn't constant, though its initial value is a
    // literal.
    // expect-warning: Variable used before its declaration
    // expect-reads: x -> x = 1
    argument_assigns_variable { |a = (x = 5)| var x = 1; ^x }
    // expect-warning: Variable used before its declaration
    // expect-reads: x -> x = 1
    argument_captures_variable { |a = { x }| var x = 1; ^x }
    // A declaration after an expression isn't in the frame, the old compiler didn't allow one, so it is an error, and
    // 3.16's rules apply.
    // expect-error: Variable declared after an expression
    // expect-error: Variable not defined
    after_expression { 1; var a = b; var b = 2; ^a }
    // A generator's qualifiers aren't in the frame either, so the name is what it hides, as in the old compiler.
    // expect-error: Variable not defined
    generator { ^{: y, var y = y } }
    // expect: no diagnostics
    // expect-reads: y -> var y = y * 2; y <- [1, 2]
    generator_rebinds { ^{: y, y <- [1, 2], var y = y * 2 } }
    // The old compiler made the qualifier `{ |y| y.yield }.value({ y })`, so the function is outside the block declaring
    // `y`, and is the outer `y`, unlike 3.16.
    // expect-warning: Qualifier's variable used in its own value
    // expect-reads: y -> var y = { y }; y = \outer
    generator_function_in_value { var y = \outer; ^{: y, var y = { y } } }
    // With nothing outside, it isn't defined, the old compiler read the Interpreter's `y` in interpreted code.
    // expect-error: Variable not defined
    generator_function_in_value_undefined { ^{: y, var y = { y } } }
    // The frame's `foo` is nil while its initial value runs, so this is `nil + 1`, unlike 3.16.
    // expect-warning: Variable used before its declaration
    // expect-reads: foo -> foo = foo + 1; foo = foo + 1
    shadows_outer_expression { var foo = 10; ^{ var foo = foo + 1; foo == 11 } }
    // expect-warning: Variable used before its declaration
    // expect: no result
    nested_return { |x| var a = { if (x) { ^b }; ^2 }, b = 1; ^a }
}

LookupThreeSixteen {
    //# VERSION 3.16

    // expect-error: Variable not defined
    // expect: poisoned
    argument_sees_computed_variable {
        ^{
            arg a1 = x1;
            var x1 =2 + 2;
            a1
        }.()
    }

    // expect-error: Variable not defined
    // expect: poisoned
    argument_sees_literal_variable {
        ^{
            arg a1 = x1;
            var x1 = 4;
            a1
        }.()
    }

    // expect-error: Variable not defined
    self_reference { var f = f; ^f }
    // expect: no diagnostics
    // expect-reads: foo -> foo = 10; foo = foo + 1
    shadows_outer_expression { var foo = 10; ^{ var foo = foo + 1; foo == 11 } }
    // expect-error: Variable not defined
    later_variable { var a = b, b = 1; ^a }
    // expect-reads: x -> x = 1
    hides_outer { var x = 1; ^{ var a = x, x = 2; a } }
    // expect-error: Variable not defined
    later_argument { arg a = b, b = 1; ^a }
    // expect: no diagnostics
    // expect-reads: y -> var y = { y }; var y = { y }
    generator_function_in_value { var y = \outer; ^{: y, var y = { y } } }
}
