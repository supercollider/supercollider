// bytecode
// Legacy sends get the bytecodes the old compiler chose, by how they are written, checked against old sclang's.
// Each method starts with `cv;` so the old compiler compiles it, rather than replacing a one line method with a
// shortcut, e.g., one returning an instance variable.
BytecodeSends {
    var inst;
    classvar cv;

    // A message to `this` doesn't push it.
    thisOpt { cv; ^this.foo }
    specialThisOpt { cv; ^this.size }
    // A special message to an instance variable pushes it and sends with one bytecode.
    instanceSpecial { cv; ^inst.size }

    // The function's own arguments, in order, are pushed with the send, all of them, or all but the receiver, or the
    // first two pushed.
    pushAllArgs { |a, b| cv; ^this.foo(a, b) }
    pushAllButFirst { |a, b| cv; ^inst.size.foo(a, b) }
    pushAllButFirstTwo { |a, b| cv; ^this.foo(1, a, b) }
    specialPushAllArgs { |a, b| cv; ^this.put(a, b) }
    // A name that isn't the first argument doesn't count, only other expressions do.
    notTheArguments { |a, b| cv; ^inst.foo(a, b) }

    // `(a..b).do(f)` is one forSeries, `series` written as a selector isn't.
    forSeries { |f| cv; ^(1..10).do(f) }
    seriesSelector { |f| cv; ^1.series(nil, 10).do(f) }

    // Operators: unary, the unary with an argument as a message, a binary with an adverb, and `+ 1` and `- 1`.
    unary { |a| cv; ^a.neg }
    unaryWithArgument { |a| cv; ^a.neg(1) }
    unaryWrittenBinary { |a| cv; ^a neg: 1 }
    binary { |a| cv; ^a * 3 }
    binaryNotNibble { |a| cv; ^a pow: 3 }
    adverb { |a| cv; ^a +.x 3 }
    addOne { |a| cv; ^a + 1 }
    subtractOne { |a| cv; ^a - 1 }

    // Keyword arguments, with the selector added after the arguments for an operator.
    keywords { |a| cv; ^a.foo(1, x: 2) }
    specialKeywords { |a| cv; ^a.at(1, x: 2) }
    operatorKeywords { |a| cv; ^a.max(1, x: 2) }
    manyArguments { |a| cv; ^a.foo(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) }

    superThisOpt { cv; ^super.foo }
    superArguments { |a| cv; ^super.foo(a) }
    superKeywords { |a| cv; ^super.foo(a, x: 1) }
    setter { |a| cv; a.foo = 1 }
    superSetter { cv; super.foo = 1 }

    // A selector is added before its arguments' functions and classes, a setter's after.
    tableOrder { |a| cv; ^a.foo({ 1 }, Interpreter) }
    setterTableOrder { |a| cv; a.foo = { 1 } }

    environment { cv; ^~x }
    performList { |a, b| cv; ^a.foo(*b) }
    // A function's last message is a tail call, as is `^`'s.
    tail { cv; ^{ |a| a.foo } }
}
