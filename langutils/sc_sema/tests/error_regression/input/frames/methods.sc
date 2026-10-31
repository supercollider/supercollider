// frames
//# VERSION 3.16
// A method's frame is `this`, its arguments, its variadic arguments, then its variables in the order declared. A
// function has its own frame, so reading its parent's variables is one frame up. Members aren't in frames.
FrameMembers {
    var instance, other;
    classvar shared;
    const constant = 1;

    // expect: no diagnostics
    arguments { |a, b ...rest| var x = a; var y = b; ^[x, y, rest] }
    // expect: no diagnostics
    nested { |a| var x; ^{ |b| var y = a + b; { x = y } } }
    // A function's variable that hides its parent's is in its own frame.
    // expect: no diagnostics
    shadowing { var x = 1; ^{ var x = x + 1; x } }
    // expect: no diagnostics
    pack { var #p, q ...r = [1, 2, 3]; ^p + q }
    // expect: no diagnostics
    members { instance = other; shared = instance; ^constant }
    // expect: no diagnostics
    nested_return { |x| if (x) { ^1 }; ^2 }
    // A class method's frame also starts with `this`, the class.
    // expect: no diagnostics
    *classMethod { |a| ^shared + a }
}
