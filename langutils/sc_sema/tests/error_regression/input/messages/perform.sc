//# VERSION 3.16
Perform {
    postfix { ^1.perform(\foo, 2) }
    postfix_selector_only { ^1.perform(\foo) }
    postfix_quoted_symbol { ^1.perform('foo', 2) }
    prefix { ^perform(1, \foo, 2) }
    binary { ^1 perform: \foo }
    not_a_symbol { ^1.perform(2, 3) }
    not_constant { ^1.perform(\foo.bar, 2) }
    not_first_argument { ^1.perform(2, \foo) }
    keyword_argument { ^1.perform(selector: \foo) }
}
