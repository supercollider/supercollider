// bytecode
// Legacy values get the bytecodes, and constants table entries, the old compiler gave them, checked against old
// sclang's. Each method starts with `cv;` so the old compiler compiles it, rather than replacing a one line method with
// a shortcut.
BytecodeValues {
    var inst, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16;
    classvar cv;
    const kInteger = 2, kString = "s", kFloat = 1.5;

    // Small numbers have their own bytecodes, other integers are in the bytecode, other floats are constants, each
    // once, compared as numbers.
    numbers { cv; ^[0, 1, 2, -1, 3, 300, 70000, 9000000, 0.5, -1.0, 0.0, 1.0, 2.0, inf, 1.25, -0.0, 1.25] }
    // Symbols and characters are shared, each string is its own object.
    symbolsAndStrings { cv; ^[\a, \a, "s", "s", $c, $c, #[1, 2]] }
    // A class constant is always a constant, the class's object, shared with itself and equal numbers.
    classConstants { cv; ^[kInteger, kInteger, kString, kString, kFloat, 1.5] }
    classes { cv; ^[Object, Array, Interpreter] }
    pseudoVariables { cv; ^[thisProcess, thisThread, thisMethod, thisFunctionDef, thisFunction] }

    returnNil { cv; ^nil }
    returnTrue { cv; ^true }
    returnFalse { cv; ^false }
    returnThis { cv; ^this }
    returnSelf { cv; 1 }

    // Legacy code doesn't fold, so a variable is read, even when its value is known.
    knownVariable { var b = 1; cv; ^b }
    // A store before a drop pops when it has a short bytecode.
    stores { |a| var b; cv; b = a; inst = a; cv = a; ^b = a }
    highSlots { var v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16; cv; v16 = 1; ^v16 }
    highInstanceVariable { cv; i16 = 1; ^i16 }
    deepRead { |a| cv; ^{ { { { { { { { { a } } } } } } } } } }
    deepStore { |a| cv; ^{ { { { { { { { { a = 1; 2 } } } } } } } } } }

    // A computed default runs when the argument is nil, a literal one is in the frame.
    defaults { |a = (1 + 2), b = 3| cv; ^a }
    computedVariable { var a = (1 + 2); cv; ^a }
    pack { |a| var b, c; cv; #b, c = a; ^b }
}
