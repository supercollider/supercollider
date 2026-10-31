// compiled
// A partial application's function is only its call, never the brackets around it, so `((_ + 1))`'s source code is
// `_ + 1`. The old compiler included an expression's brackets, but not a declaration's, e.g., `var a (_ + 1)`.
PartialApplicationBrackets {
    // expect: no diagnostics
    none { ^_ + 1 }
    brackets { ^(_ + 1) }
    nested { ^((_ + 1)) }
    spaces_and_comments { ^( /* c */ _ + 1 ; ) }
    nested_call { ^(_ + (_ + (_)) + _) }
    sequence { ^(1; _ + 1) }
    multiplied { ^(_ + 1) * 2 }
    in_function { ^{ (_.foo(2)) } }
    variable { var a (_ + 1), b = (_ + 1), c ((_ + 1)); ^[a, b, c] }
    arguments { |a (_ + 1), b = (_ + 1)| ^[a, b] }
    non_local_return { |x| if (x) { ^(_ + 1) }; ^2 }
}
