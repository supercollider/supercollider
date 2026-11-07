//# VERSION 3.16
// A closed function, `#{ }`, only sees its own variables and the Interpreter's members, e.g., `a` to `z`.
ClosedLookup {
    var instance_var;
    classvar class_var;

    // expect-reads: a -> a
    interpreter_member { ^#{ a } }
    // expect: no diagnostics
    assigns_interpreter_member { ^#{ z = 1 } }
    // expect: no diagnostics
    pack_assigns_interpreter_members { ^#{ #a, b = [1, 2] } }
    // expect-reads: cmdLine -> cmdLine
    interpreter_private_member { ^#{ cmdLine } }
    // expect-reads: x -> x
    own_argument { ^#{ |x| x } }
    // expect-reads: v -> v = 1
    own_variable { ^#{ var v = 1; v } }
    // expect-reads: x -> x
    nested_open_sees_closed { ^#{ |x| { x } } }
    // The argument hides the Interpreter's member.
    // expect-reads: a -> a
    argument_hides_member { ^#{ |a| a } }
    // expect: no diagnostics
    special_names { ^#{ [this, thisProcess, thisThread, thisFunction, thisFunctionDef] } }

    // expect-error: Variable outside closed function
    outer_variable { var foo = 10; ^#{ foo } }
    // expect-error: Variable outside closed function
    outer_argument { |foo| ^#{ foo } }
    // expect-error: Variable outside closed function
    instance_variable { ^#{ instance_var } }
    // expect-error: Variable outside closed function
    class_variable { ^#{ class_var } }
    // expect-error: Variable outside closed function
    assigns_outer { var foo; ^#{ foo = 1 } }
    // expect-error: Variable outside closed function
    pack_assigns_outer { var foo; ^#{ #foo, a = [1, 2] } }
    // expect-error: Variable outside closed function
    outer_function_variable { ^{ |foo| #{ foo } } }
    // expect-error: Variable outside closed function
    outer_closed_variable { ^#{ |foo| #{ foo } } }
    // A function in its own initial value could see it, a closed one can't.
    // expect-error: Variable outside closed function
    own_initial_value { var foo = #{ foo }; ^foo }
    // `f` is also the Interpreter's member, which is what the closed function reads.
    // expect-reads: f -> f; f = #{ f }
    own_initial_value_interpreter_name { var f = #{ f }; ^f }
    // expect-error: Variable not defined
    undefined { ^#{ nothing } }

    // `a` in the closed function is the Interpreter's, so the variable isn't captured, and stays constant.
    // expect-reads: a -> a; a = 1
    outer_not_captured { var a = 1; #{ a }; ^a }
    // An open function does capture it.
    // expect-reads: a -> a = 1; a = 1
    open_captures { var a = 1; { a }; ^a }
}
