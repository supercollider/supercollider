//# VERSION 3.16
Dictionaries {
    constant { ^(a: 1, b: 2.5) }
    empty { ^() }
    symbol_key { ^(\a: 1) }
    string_key { ^("a": 1) }
    integer_key { ^(1: \one) }
    nested_array { ^(a: [1, 2]) }
    nested_event { ^(a: (b: 1)) }
    value_not_constant { ^(a: 1.foo) }
    key_not_constant { ^((1.foo): 2) }
    nested_not_constant { ^(a: (b: 1.foo)) }
}
