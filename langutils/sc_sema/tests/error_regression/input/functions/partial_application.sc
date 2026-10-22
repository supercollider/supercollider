PartialApplication {
    var member;

    // expect: no diagnostics
    binary_one { ^_ + 1 }
    binary_two { ^_ + _ }
    receiver { ^_.foo }
    arguments { ^member.foo(_, _, _) }
    receiver_and_arguments { ^_.foo(_) }
    prefix { ^foo(_, 1) }
    keyword { ^member.foo(1, a: _) }
    variadic { ^member.foo(*_) }
    value_message { ^_.(1) }
    new_message { ^Array(_) }
    at { ^member[_] }
    at_receiver { ^_[0] }
    slice { ^member[1.._] }
    put { ^member[_] = _ }
    setter { ^_.foo = 1 }
    range { ^(1.._) }
    array { ^[_, 1] }
    collection { ^Array[_, 2] }
    dictionary { ^(a: _) }
    // expect: no diagnostics
    reference { ^`_ }
    // expect-reads: _ -> _
    nested_calls_are_separate { ^member.foo(_, member.bar(_)) }
    brackets_end_it { ^(_ + 1) * 2 }
    // expect-reads: a -> a = 1
    // expect: no result
    captures_local { var a = 1; ^a + _ }
    // expect-reads: x -> x
    in_block { ^{ |x| x + _ } }
    nested_non_local_return { ^_.collect { |x| ^x } }

    // expect-error: Partial application outside of a call
    // expect: poisoned
    alone { ^_ }
    // expect-error: Partial application outside of a call
    assigned { var a = _; ^a }
    // expect-error: Partial application outside of a call
    inside_other_expression { ^member.foo({ _ }) }
}
