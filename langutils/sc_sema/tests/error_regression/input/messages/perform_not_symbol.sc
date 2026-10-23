//# VERSION 3.16
PerformNotSymbol {
    integer { ^1.perform(2) }
    float { ^1.perform(2.5, 3) }
    string { ^1.perform("foo") }
    string_with_quote { ^1.perform("it's") }
    char { ^1.perform($a) }
    boolean { ^1.perform(true) }
    nil_selector { ^1.perform(nil) }
    class_name { ^1.perform(Object) }
    prefix { ^perform(1, 2) }
    binary { ^1 perform: 2 }
    not_constant { ^1.perform(2.bar) }
}
