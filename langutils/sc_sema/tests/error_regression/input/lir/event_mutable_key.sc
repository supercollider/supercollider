// lir
//# VERSION 3.16
// Events compare their keys by identity, so a copy of a folded event couldn't replace a mutable key, it would add a
// second entry. Such an event isn't folded, it is made each time. A mutable value is still folded and copied.
LirEventMutableKey {
    // expect: no result
    array_key { ^([1, 2]: 3) }
    // expect: no result
    string_key { ^("a": 3) }
    // expect: no result
    event_class { ^Event[[1], 2] }
    // expect: -> (a: [4])
    mutable_value { ^(a: [4]) }
    // expect: -> (a: 3)
    symbol_key { ^(a: 3) }
}
