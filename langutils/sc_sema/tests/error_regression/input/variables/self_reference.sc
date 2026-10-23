//# VERSION 3.16
// A variable can refer to itself from a function in its initial value, as the function is called after the variable
// is assigned, e.g., for recursion. Anywhere else in its initial value it hasn't been assigned yet, so it is an error.
// Variables can be declared anywhere in a body, and can't be used before their declaration.
SelfReference {
    var member;

    // expect-error: Variable used in its own initial value
    // expect: poisoned
    direct { var f = f; ^f }
    // expect-error: Variable used in its own initial value
    in_expression { var f = f + 1; ^f }
    // expect-error: Variable used in its own initial value
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
    // expect-error: Variable used in its own initial value
    after_expressions_direct { 1; var f = f; ^f }
    // expect: no diagnostics
    // expect-reads: g -> g = { g }
    declared_in_nested_body { ^{ 1; var g = { g }; g } }
    // expect-error: Variable used in its own initial value
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

    // The new variable hides the one it shares a name with, even in its own initial value.
    // expect-reads: f -> f = { f }
    shadows_outer { var f = 1; ^{ var f = { f }; f } }
    // expect-error: Variable used in its own initial value
    shadows_outer_direct { var f = 1; ^{ var f = f; f } }
    // expect-error: Variable used in its own initial value
    shadows_member_direct { var member = member; ^member }

    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    argument { |f = { f }| ^f }
    // expect-error: Variable used in its own initial value
    argument_direct { |f = (f)| ^f }
    // expect: no diagnostics
    // expect-reads: f -> f = { f }
    block_argument { ^{ |f = { f }| f } }

    // expect: no diagnostics
    // expect-reads: a -> a
    // expect-reads: b -> b
    pack { var #a, b = [{ a }, { b }]; ^a }
    // expect-error: Variable used in its own initial value
    pack_direct { var #a, b = [a]; ^a }

    // A Routine runs later, so it is a function, an immediate generator runs now.
    // expect: no diagnostics
    routine_generator { var r = {: r, x <- [1] }; ^r }
    // expect-error: Variable used in its own initial value
    immediate_generator { var r = {; r, x <- [1] }; ^r }
    // expect: no diagnostics
    // expect-reads: y -> var y = { y }
    generator_qualifier { ^{: y, var y = { y } } }
    // expect-error: Variable used in its own initial value
    generator_qualifier_direct { ^{: y, var y = y } }

    // `f.foo(_)` is `{ |_| f.foo(_) }`, so f is read later.
    // expect: no diagnostics
    partial_application { var f = f.foo(_); ^f }
}
