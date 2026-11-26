// bytecode
// Legacy inlining compiles `if`, `while` and the others as jumps, without their functions, where the old compiler did,
// checked against old sclang's bytes. Only functions without arguments or variables are inlined. Each method starts
// with `cv;` so the old compiler compiles it, rather than replacing a one line method with a shortcut.
BytecodeInlining {
    var inst;
    classvar cv;

    // An inlined body is in the method's frame, so `^` in it is a plain return, and its last send is a tail call
    // when the `if` is.
    ifThen { |a| cv; if (a) { ^1 }; ^2 }
    ifThenElse { |a| cv; ^if (a) { a.foo } { a.bar } }
    ifMessage { |a| cv; ^a.if({ 1 }, { 2 }) }
    ifKeywordBinary { |a| cv; ^a if: { 1 } }
    // Empty bodies have their own forms.
    ifEmpty { |a| cv; ^if (a) { } }
    ifEmptyThen { |a| cv; ^if (a) { } { 2 } }
    ifEmptyElse { |a| cv; ^if (a) { 1 } { } }
    ifBothEmpty { |a| cv; ^if (a) { } { } }
    // `x.isNil` and `x.notNil` are tested by the jump, isNil isn't sent.
    ifIsNil { |a| cv; ^if (a.isNil) { 1 } }
    ifNotNil { |a| cv; ^if (a.notNil) { 1 } { 2 } }
    ifIsNilEmpty { |a| cv; ^if (isNil(a)) { } { 2 } }
    // Not inlined: a function with an argument or variable, a value that isn't a function, a keyword argument.
    ifArguments { |a| cv; ^if (a) { |x| x } }
    ifVariables { |a| cv; ^if (a) { var x = 1; x } }
    ifValue { |a, b| cv; ^if (a, b) }
    ifKeyword { |a| cv; ^a.if(true: { 1 }) }
    // When not inlined, it is sent without the shorter bytecodes, here pushing all arguments.
    ifPlain { |a, b| cv; ^this.if(a, b) }

    andOr { |a, b| cv; ^(a and: { b.foo }) or: { b.bar } }
    andNotInlined { |a, b| cv; ^a and: b }
    nilCoalesce { |a| cv; ^a ?? { 1 } }
    notNilThen { |a| cv; ^a !? { a.foo } }
    questionMark { |a| cv; ^a ? 1 }

    // A loop's body is never a tail call, its value is dropped by the jump back.
    whileBody { |a| cv; while { a.notEmpty } { a.pop }; ^a }
    whileCondition { |a| cv; while { a.pop.notNil }; ^a }
    whileEmptyBody { |a| cv; while { a.next } { }; ^a }
    whileTrue { |a| cv; true.while { if (a.next) { ^a } } }
    // `{ true }` isn't treated as true.
    whileTrueFunction { |a| cv; { true }.while { if (a.next) { ^a } } }
    loop { |a| cv; loop { if (a.next) { ^a } } }

    // Nested ifs, a condition of only `x.isNil` tested as the `if` would.
    case { |a| cv; ^case { a == 1 } { \one } { a.isNil } { \nil } { \other } }
    caseNoDefault { |a| cv; ^case { a == 1 } { \one } { a == 2 } { \two } }
    caseConditionStatements { |a| cv; ^case { a.foo; a.bar } { 1 } }
    caseNotInlined { |a, f| cv; ^case({ a }, { 1 }, f) }

    // A table of literal keys, or functions returning one, to offsets. Nil is the default when there isn't one.
    switch { |a| cv; ^switch (a, 1, { \one }, \two, { a.foo }, $c, { }, { \default }) }
    switchNoDefault { |a| cv; ^switch (a) { 1 } { \one } { 2.0 } { \two } }
    switchFunctionKey { |a| cv; ^switch (a, { 1 }, { \one }, { \other }) }
    // A string isn't a key, nor is nil.
    switchString { |a| cv; ^switch (a, "s", { 1 }) }
    switchNil { |a| cv; ^switch (a, nil, { 1 }) }

    // Functions inside an inlined body are one frame down from the method.
    nestedFunction { |a| cv; ^if (a) { { a } } }
}

// Where the old compiler's bytes were wrong, which differ from old sclang's on purpose.
BytecodeInliningFixes {
    classvar cv;

    // An empty body is nil, the old compiler left the stack without a value.
    andEmpty { |a| cv; ^a and: { } }
    whileEmptyCondition { cv; while { }; ^nil }
    loopEmpty { cv; loop { } }
    caseEmptyCondition { cv; ^case { } { 1 } }
    // A closed function is compiled in the Interpreter, so it isn't inlined.
    closed { |a| cv; ^if (a) #{ 1 } }
    // The first of equal keys is used, as Object:switch does, the old compiler used the last, leaving the first's
    // jump unpatched.
    switchDuplicateKeys { |a| cv; ^switch (a, 1, { \first }, 1, { \second }) }
}
