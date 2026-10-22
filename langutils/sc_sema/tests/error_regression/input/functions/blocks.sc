Blocks {
    var instance_var;

    empty { ^{} }
    constant_body { ^{ 1 } }
    // expect-reads: a -> a
    // expect-reads: b -> b
    pipe_arguments { ^{ |a, b| a + b } }
    // expect-reads: a -> a
    arg_arguments { ^{ arg a, b; a } }
    // expect-reads: rest -> rest
    variadic { ^{ |a ... rest| rest } }
    // expect-reads: a -> a = 1
    default_argument { ^{ |a = 1| a } }
    // expect-reads: instance_var -> instance_var
    reads_instance { ^{ instance_var } }
    assigns_argument { ^{ |a| a = 2 } }
    // expect-reads: a -> a
    // expect-reads: b -> b
    nested { ^{ |a| { |b| a + b } } }
    // expect-reads: instance_var -> instance_var
    shadowing { ^{ |instance_var| instance_var } }
    // expect-error: Variable not defined
    // expect-reads: a -> a
    out_of_scope { ^{ |a| a }.value(1) + a }
    // expect-reads: x -> x
    trailing_block { ^1.do { |x| x } }
    // expect: no diagnostics
    non_local_return { ^{ ^1 } }
    multiple_expressions { ^{ 1; 2 } }
}
