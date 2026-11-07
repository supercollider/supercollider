//# VERSION 3.16
EventKeyWithoutValue {
    odd_constant { ^Event[a: 1, \b] }
    odd_not_constant { ^Event[a: 1.foo, \b] }
    single { ^Event[\a] }
    even { ^Event[a: 1, b: 2] }
    array_odd_is_fine { ^Array[1, 2, 3] }
}
