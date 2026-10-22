Messages {
    postfix { ^1.foo }
    postfix_args { ^1.foo(2, 3) }
    postfix_keyword { ^1.foo(2, bar: 3) }
    postfix_variadic { ^1.foo(2, *3) }
    prefix { ^foo(1, 2) }
    prefix_no_receiver { ^foo(bar: 1) }
    binary { ^1 + 2 }
    binary_keyword { ^1 max: 2 }
    binary_adverb_name { ^1 +.x 2 }
    binary_adverb_int { ^1 +.1 2 }
    binary_adverb_expr { ^1 +.(3) 2 }
    value { ^1.(2) }
    value_empty { ^1.() }
    new_message { ^Object(1) }
    new_message_empty { ^Object() }
    at { ^"abc"[0] }
    at_range { ^"abc"[0..1] }
    nested { ^1.foo(2.bar) + 3 }
}
