Blocks {
    var instance_var;

    empty { ^{} }
    constant_body { ^{ 1 } }
    pipe_arguments { ^{ |a, b| a + b } }
    arg_arguments { ^{ arg a, b; a } }
    variadic { ^{ |a ... rest| rest } }
    default_argument { ^{ |a = 1| a } }
    reads_instance { ^{ instance_var } }
    assigns_argument { ^{ |a| a = 2 } }
    nested { ^{ |a| { |b| a + b } } }
    shadowing { ^{ |instance_var| instance_var } }
    out_of_scope { ^{ |a| a }.value(1) + a }
    trailing_block { ^1.do { |x| x } }
    non_local_return { ^{ ^1 } }
    multiple_expressions { ^{ 1; 2 } }
}
