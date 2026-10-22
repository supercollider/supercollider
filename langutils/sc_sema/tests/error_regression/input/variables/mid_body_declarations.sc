// Variables can be declared anywhere in a body, not just at its top. A variable is in scope from its declaration to
// the end of its body, so a name read before it is declared refers to whatever it hid, or isn't defined.
MidBodyDeclarations {
    var member;

    // expect: -> 2
    // expect: no diagnostics
    after_expression { 1; var a = 2; ^a }
    // expect: -> 1
    // expect: no diagnostics
    interleaved { var a = 1; 10; var b = a; 20; var c = b; ^c }
    // expect: no diagnostics
    // expect-reads: a -> a = member
    after_message { member.postln; var a = member; ^a }
    // expect: no result
    // expect-reads: a -> a = 1
    after_assignment { var a = 1; a = a + 1; var b = a; ^b }
    // expect: no diagnostics
    // expect-reads: b -> b
    without_value { 1; var b; b = 2; ^b }
    // expect: no diagnostics
    // expect-reads: x -> x
    // expect-reads: y -> y
    pack { 1; var #x, y = [1, 2]; ^x + y }

    // expect-error: Variable not defined
    used_before_declaration { a.postln; var a = 1; ^a }
    // expect-error: Variable not defined
    assigned_before_declaration { a = 2; var a = 1; ^a }
    // expect-error: Variable already declared
    redeclared_later { var a = 1; 1; var a = 2; ^a }

    // Before its declaration a name reads the variable it will hide.
    // expect-reads: x -> x = 1; x = 2
    hides_outer_after_read { var x = 1; ^{ x.postln; var x = 2; x } }
    // expect-reads: member -> member; member = 2
    hides_member_after_read { member.postln; var member = 2; ^member }

    // expect: no diagnostics
    // expect-reads: g -> g = 2
    in_nested_function { ^{ 1; var g = 2; g } }
    // expect: no diagnostics
    // expect-reads: g -> g = 1
    // expect-reads: h -> h = g
    in_deeply_nested_function { ^{ 1; { 2; var g = 1; 3; var h = g; h } } }
    // expect: no diagnostics
    // expect-reads: g -> g = 1
    captured_by_later_function { 1; var g = 1; var f = { g }; ^f }
    // expect: no diagnostics
    after_argument { |a| a.postln; var b = a; ^b }
    // expect: no diagnostics
    in_block_after_argument { ^{ |a| a.postln; var b = a; b } }

    // expect: no diagnostics
    // expect-reads: a -> a = 1
    after_non_local_return { |x| if (x) { ^1 }; var a = 1; ^a }
    // expect: no diagnostics
    // expect-reads: a -> a = 1; a = 2
    non_local_return_in_nested_body { |x| ^{ if (x) { var a = 1; ^a }; var a = 2; a } }
}
