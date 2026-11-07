// bytecode
// A function is closed, with no context, unless it or a function in it uses a variable of a frame around it, `this`,
// `super`, `thisFunction`, `^` or a member. A frame creating a function with a context is garbage collected (heap
// context). Checked against old sclang. Each method starts with `cv;` so it isn't replaced with a shortcut.
BytecodeProperties {
    const kConst = 1;
    var inst;
    classvar cv;

    // expect: no diagnostics
    closed { cv; ^{ |x| x + 1 } }
    // expect: no diagnostics
    outerArgument { |a| cv; ^{ a } }
    // expect: no diagnostics
    outerAssign { |a| cv; ^{ a = 1 } }
    // expect: no diagnostics
    twoDeep { |a| cv; ^{ { a } } }
    // expect: no diagnostics
    nestedClosed { |a| cv; ^{ { 1 } } }
    // expect: no diagnostics
    oneOut { cv; ^{ |b| { b } } }
    // expect: no diagnostics
    this_ { cv; ^{ this } }
    // expect: no diagnostics
    thisNested { cv; ^{ { this } } }
    // expect: no diagnostics
    super_ { cv; ^{ super.foo } }
    // expect: no diagnostics
    thisFunction_ { cv; ^{ thisFunction } }
    // expect: no diagnostics
    thisMethod_ { cv; ^{ thisMethod } }
    // expect: no diagnostics
    return { cv; ^{ ^1 } }
    // expect: no diagnostics
    inlinedReturn { |a| cv; ^{ if (a) { ^1 }; 2 } }
    // expect: no diagnostics
    instanceVariable { cv; ^{ inst } }
    // expect: no diagnostics
    classVariable { cv; ^{ cv } }
    // expect: no diagnostics
    constant { cv; ^{ kConst } }
    // A function in an inlined body is created by the method's frame.
    // expect: no diagnostics
    inInlined { |a| cv; ^if (a) { { a } } }
    // expect: no diagnostics
    partial { |a| cv; ^[_ + a, _ + 1] }
    // expect: no diagnostics
    defaultArgument { |a| cv; ^{ |x = (a)| x } }
    // An explicitly closed function never has a context, but isn't closed if it uses `this`, nor is a function around it.
    // expect: no diagnostics
    explicit { cv; ^[#{ 1 }, { #{ this } }] }
}

+ Interpreter {
    // The Interpreter's members don't stop a function being closed.
    bytecodeProperties { nil; ^[{ a }, { this }] }
}
