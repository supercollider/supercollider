//# VERSION 3.16
// Each class records its superclass, Object when none is written, and a meta class's is its superclass's meta class.
// Each method records its selector, including binary operators, setters, class methods and extensions.
SelectorBase {
    // expect: -> 1
    plain { ^1 }
    // expect: no diagnostics
    + { |other| ^other }
    // expect: no diagnostics
    value_ { |v| ^v }
    // expect: -> 2
    *classMethod { ^2 }
}

SelectorDerived : SelectorBase {
    // expect: no diagnostics
    nested_return { |x| if (x) { ^1 }; ^2 }
}

+ SelectorDerived {
    // expect: -> 3
    extended { ^3 }
}
