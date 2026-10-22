ArgumentDefaults {
    var instance_var;

    // expect-reads: a -> a = 1
    block_constant { ^{ |a = 1| a } }
    // expect-reads: a -> a = #[1, 2]
    block_constant_array { ^{ |a = #[1, 2], b = (c: 3)| a } }
    // expect-reads: a -> a = (1.foo)
    block_bracketed { ^{ |a = (1.foo)| a } }
    // expect-reads: a -> a
    // expect-reads: b -> b = (a + 1)
    block_earlier_argument { ^{ |a, b = (a + 1)| b } }
    // expect-error: Variable not defined
    block_later_argument { ^{ |a = (b), b| a } }
    // expect-error: Variable not defined
    block_itself { ^{ |a = (a)| a } }

    // expect: no result
    method_constant { |a = 1| ^1 }
    // expect-reads: instance_var -> instance_var
    method_instance_var { |a = (instance_var)| ^1 }
    // expect-reads: a -> a
    method_other_argument { |a, b = (a)| ^1 }
}
