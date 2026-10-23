//# VERSION 3.16
MethodArguments {
    var instance_var;

    // expect-reads: a -> a
    // expect: no result
    read_argument { |a| ^a }
    assign_argument { |a| a = 1 }
    // expect-reads: a -> a
    // expect-reads: b -> b = (a + 1)
    earlier_argument { |a, b = (a + 1)| ^b }
    // expect-error: Variable not defined
    later_argument { |a = (b), b| ^a }
    // expect-reads: instance_var -> instance_var
    shadows_instance { |instance_var| ^instance_var }
    // expect-reads: rest -> rest
    variadic { |a ... rest| ^rest }
    // expect-reads: a -> a
    block_uses_argument { |a| ^{ a } }

    // expect-reads: a -> a
    // expect-reads: b -> b = (a)
    *class_read_argument { |a, b = (a)| ^b }
}
