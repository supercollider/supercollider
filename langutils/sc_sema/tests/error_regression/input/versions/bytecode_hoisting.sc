// bytecode
// Legacy only inlines functions without variables, as the old compiler did, see bytecode/hoisting for 3.16.
BytecodeHoistingLegacy {
    ifVar { |a| ^if (a) { var c = a.foo; c } { 2 } }
    loop { |a| while { a.next } { var c; c = c ? 0 + 1 } }
    nonLocalReturn { |a| if (a) { var c = a.foo; if (c) { ^1 }; ^c }; ^2 }
    captured { |a| ^if (a) { var c = 1; { c } } }
}
