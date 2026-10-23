//# VERSION 3.16
// A variable can refer to itself from a function in its initial value, as the function is called after the variable
// is assigned, e.g., for recursion. Anywhere else in its initial value it hasn't been assigned yet, so its name refers
// to what it hides, e.g., `var x = x + 1` reads the outer `x`, or isn't defined. Variables can be declared anywhere in
// a body, and can't be used before their declaration.
SelfReference {
    var member;

    // expect-error: Variable not defined
    // expect: poisoned
    direct { var f = f; ^f }
    // expect-error: Variable not defined
    in_expression { var f = f + 1; ^f }
    // expect-error: Variable not defined
    assigned_directly { var f = (f = 1); ^f }

    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    in_function { var f = { f }; ^f }
    // expect: no diagnostics
    // expect-reads: f -> f = { { f } }
    in_nested_function { var f = { { f } }; ^f }
    // expect: no diagnostics
    // expect-reads: fact -> fact = { |n| if (n <= 1) { 1 } { n * fact.(n - 1) } }
    recursion { var fact = { |n| if (n <= 1) { 1 } { n * fact.(n - 1) } }; ^fact.(5) }
    // expect: no diagnostics
    assigned_in_function { var f = { f = nil }; ^f }
    // expect: no result
    not_constant { var f = { f }; ^f }

    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    after_expressions { 1; member.postln; var f = { f }; ^f }
    // expect-error: Variable not defined
    after_expressions_direct { 1; var f = f; ^f }
    // expect: no diagnostics
    // expect-reads: g -> g = { g }
    declared_in_nested_body { ^{ 1; var g = { g }; g } }
    // expect-error: Variable not defined
    declared_in_nested_body_direct { ^{ 1; var g = g; g } }
    // expect: no diagnostics
    // expect-reads: g -> g = { { g } }
    deeply_nested { ^{ { var g = { { g } }; g } } }

    // expect-error: Variable not defined
    forward_reference { var a = { b }; var b = 1; ^a }
    // expect-error: Variable not defined
    forward_in_same_declaration { var a = { b }, b = 1; ^a }
    // expect-error: Variable not defined
    used_before_declaration { var a = b; var b = 1; ^a }

    // The new variable hides the one it shares a name with in a function in its own initial value, but elsewhere in it
    // the name is still the one it hides.
    // expect-reads: f -> f = { f }
    shadows_outer { var f = 1; ^{ var f = { f }; f } }
    // expect: no diagnostics
    // expect-reads: f -> f = 1; f = f
    shadows_outer_direct { var f = 1; ^{ var f = f; f } }
    // expect: no diagnostics
    // expect-reads: foo -> foo = 10; foo = foo + 1
    shadows_outer_expression { var foo = 10; ^{ var foo = foo + 1; foo == 11 } }
    // expect: no diagnostics
    // expect-reads: member -> member; member = member
    shadows_member_direct { var member = member; ^member }

    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    argument { |f = { f }| ^f }
    // expect-error: Variable not defined
    argument_direct { |f = (f)| ^f }
    // expect: no diagnostics
    // expect-reads: f -> f = 1; f = (f)
    argument_shadows_outer { var f = 1; ^{ |f = (f)| f } }
    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    block_argument { ^{ |f = { f }| f } }

    // expect: no diagnostics
    // expect-reads: a -> a
    // expect-reads: b -> b
    pack { var #a, b = [{ a }, { b }]; ^a }
    // expect-error: Variable not defined
    pack_direct { var #a, b = [a]; ^a }

    // A Routine runs later, so it is a function, an immediate generator runs now.
    // expect: no diagnostics
    routine_generator { var r = {: r, x <- [1] }; ^r }
    // expect-error: Variable not defined
    immediate_generator { var r = {; r, x <- [1] }; ^r }
    // expect: no diagnostics
    // expect-reads: y -> var y = { y }
    generator_qualifier { ^{: y, var y = { y } } }
    // expect-error: Variable not defined
    generator_qualifier_direct { ^{: y, var y = y } }
    // A qualifier can rebind an earlier one's name, as in the old compiler.
    // expect: no diagnostics
    // expect-reads: y -> var y = y * 2; y <- [1, 2]
    generator_qualifier_rebinds { ^{: y, y <- [1, 2], var y = y * 2 } }

    // `f.foo(_)` is `{ |_| f.foo(_) }`, so f is read later.
    // expect: no diagnostics
    partial_application { var f = f.foo(_); ^f }
}
