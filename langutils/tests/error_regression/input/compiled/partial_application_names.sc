// compiled
// Every argument of a partial application is named `_` in argNames, as the old compiler named them, which
// FunctionDef:hasPartialApplication reads, though each read still refers to its own `_`.
PartialNames {
    // expect: no diagnostics
    one { ^_ + 1 }
    // expect: no diagnostics
    two { ^_ - _ }
    // expect: no diagnostics
    three { ^[_, _, _] }
    // expect: no diagnostics
    nested { |x| if (x) { ^_.foo(_) }; ^this.bar(_, 1, _) }
    bar { |a, b, c| ^a }
}
