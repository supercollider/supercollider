//# VERSION 3.16
// A closed function has no method to return from, even from a function inside it.
ClosedReturns {
    // expect-error: Return in closed function
    returns { ^#{ ^1 } }
    // expect-error: Return in closed function
    nested { ^#{ |x| if (x) { ^1 }; 2 } }
    // The method's own return, after a closed function, is fine.
    // expect: no diagnostics
    after { |x| if (x) { ^1 }; #{ 3 }; ^2 }
    // expect: no diagnostics
    open_inside_method { ^{ |x| if (x) { ^1 }; #{ 2 } } }
}
