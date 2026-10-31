// no-prelude
// bytecode
//# VERSION 3.16
// As Legacy's (versions/bytecode_loops). The opcodes use the method's frame, so its arguments and variables must be
// those the loop expects, with literal initial values. The old compiler reported these too, but emitted the loop.
// The prelude, with the methods of Integer and Float.
AbstractObjectExperimental {}

Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}

Class {}

String {}

True {}

False {}

Nil {}

Float {
    // expect: no diagnostics
    do { arg function;
        var i = 0.0;
        { |x| if (x) { ^1 }; ^2 }.value(function);
    }
    // Replaced by the extension below.
    reverseDo { arg function;
        var i, j=0.0;
        function.value(i, j);
    }
}

Integer {
    // expect: no diagnostics
    do { arg function;
        var i = 0;
        while { i < this } { function.value(i, i); i = i + 1 };
    }
    // expect-error: Hard coded method with a computed value
    reverseDo { arg function;
        var i = 0, j = (0);
    }
    // expect-error: Hard coded method with a computed value
    for { arg endval, function;
        var #i, j = [0, 0], stepval=1;
    }
    // expect-error: Hard coded method has the wrong arguments or variables
    forBy { arg endval, stepval, function; function.value }
}

Char {}

Symbol {}

Ref {}

Array {}

Event {}

// Interpreted code is compiled as if it were written inside functionCompileContext, so it can use these members.
Interpreter {
    var <cmdLine, filePath, lineNumber, column, context;
    var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
    var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
    var <>u, <>v, <>w, <>x, <>y, <>z;
    var <>codeDump, <>preProcessor;

    functionCompileContext {}
}

Number {
    // Variables hoisted from an inlined function are in the frame too.
    // expect-error: Hard coded method has the wrong arguments or variables
    forSeries { arg second, last, function;
        var step, j=0;
        if (second.isNil) { var extra = 1; step = extra };
    }
}

ArrayedCollection {
    // expect-error: Hard coded method has the wrong arguments or variables
    do { arg function, extra;
        var i=0;
        function.value;
    }
    // expect-error: Hard coded method has the wrong arguments or variables
    reverseDo { arg function ...rest;
        var i=0, j=0;
    }
}

Dictionary {
    // expect-error: Hard coded method with a computed value
    keysValuesArrayDo { arg argArray, function = { 1 };
        var i=0, j=0, key, val;
        var arraySize = argArray.size;
    }
}

// An extension replacing one is compiled as written, so isn't checked.
+ Float {
    // expect: no diagnostics
    reverseDo { arg function, extra;
        var i = this.size;
        function.value(i, extra);
    }
}
