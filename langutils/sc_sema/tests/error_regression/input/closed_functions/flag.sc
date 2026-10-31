//# VERSION 3.16
// `#{ }` is a closed function, `{ }` isn't, see SCFunction::closed.
ClosedFlag {
    // expect: no diagnostics
    closed { ^#{ 1 } }
    // expect: no diagnostics
    empty { ^#{} }
    // expect: no diagnostics
    with_arguments { ^#{ |a, b| } }
    // expect: no diagnostics
    open_inside_closed { ^#{ { 1 } } }
    // expect: no diagnostics
    closed_inside_open { ^{ #{ 1 } } }
    // expect: no diagnostics
    as_argument { ^1.do(#{ 2 }) }
}
