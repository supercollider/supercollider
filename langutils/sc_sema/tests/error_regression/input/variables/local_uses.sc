//# VERSION 3.16
// Whether each local is assigned or captured, shown on its declaration, see LocalVariable.
LocalUses {
    // Its initial value isn't an assignment.
    plain { var a = 1; ^a }
    assigned { var a = 1; a = 2; ^a }
    pack_assigned { var a, b; #a, b = [1, 2]; ^a }
    // A block, a partial application and a Routine generator are functions, an immediate generator isn't.
    block { var a = 1; ^{ a } }
    block_assigns { var a = 1; ^{ a = 2 } }
    partial { var a = 1; ^(a + _) }
    routine { var a = 1; ^{: a + x, x <- [1, 2] } }
    immediate { var a = 1; ^{; a + x, x <- [1, 2] } }
    // A block's own variables and arguments are used in their own frame.
    block_own { ^{ |b| var c = b; c = 2; c } }
    // An inner declaration hides the outer, so only the inner is used.
    hidden { var a = 1; ^{ var a = 2; a } }
    // A function in its own initial value captures it.
    self_reference { var f = { f }; ^f }
    arguments { |a, b| b = a; ^{ a } }
    nested_return { |x| var y = 1; if (x) { ^y }; ^x }
}
