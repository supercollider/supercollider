// no-prelude
// bytecode
// The code of these methods is replaced by extended opcodes that loop using the method's frame, as the class library's
// were. The body, and the functions in it, are thrown away. The class library's match old sclang. See bytecode/loops for
// 3.16.
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
        while ({ (i + 0.5) < this }, { function.value(i, i); i = i + 1.0; });
    }
    // A shortcut wins, as the old compiler looked for one first.
    // expect: no diagnostics
    reverseDo { ^this }
}

Integer {
    // Replaced by the extension below.
    do { arg function;
        var i = 0;
        while ({ i < this }, { function.value(i, i); i = i + 1; });
    }
    // expect: no diagnostics
    reverseDo { arg function;
        var i=0, j=0;
        i = this - 1;
        while ({ i >= 0 }, { function.value(i, j); i = i - 1; j = j + 1; });
    }
    // expect: no diagnostics
    for { arg endval, function;
        var i, j=0, stepval=1;
        i = this;
        while ({ i <= endval }, { function.value(i, j); i = i + 1; j = j + 1; });
    }
    // expect: no diagnostics
    forBy { arg endval, stepval, function;
        var i, j=0;
        i = this;
        while ({ i <= endval }, { function.value(i, j); i = i + stepval; j = j + 1; });
    }
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
    // expect: no diagnostics
    forSeries { arg second, last, function;
        var step, j=0;
        { |x| if (x) { ^1 }; ^2 }.value(second);
        ^this.forBy(last, step, function)
    }
}

ArrayedCollection {
    // expect: no diagnostics
    do { arg function;
        var i=0;
        while ({ i < this.size }, {
            function.value(this.at(i), i);
            i = i + 1;
        })
    }
    // expect: no diagnostics
    reverseDo { arg function;
        var i=0, j=0;
        i = this.size - 1;
        while ({ i >= 0 },{
            function.value(this.at(i), j);
            i = i - 1;
            j = j + 1;
        })
    }
    // A class method isn't replaced.
    // expect: no diagnostics
    *do { arg function; function.value; ^this }
}

Dictionary {
    // expect: no diagnostics
    keysValuesArrayDo { arg argArray, function;
        var i=0, j=0, key, val;
        var arraySize;
        arraySize = argArray.size;
        while ({ i < arraySize },{
            key = argArray.at(i);
            if (key.notNil, {
                val = argArray.at(i+1);
                function.value(key, val, j);
                j = j + 1;
            });
            i = i + 2;
        })
    }
    // Not replaced.
    // expect: no diagnostics
    do { arg function; function.value; ^this }
}

// An extension replacing one is replaced too, as old sclang did, with a warning.
+ Integer {
    // expect-warning: Extension of a hard coded method is ignored
    do { arg function;
        var i = 0;
        function.value(i);
    }
}
