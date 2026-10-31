//# VERSION 3.16
// Recovery that stops at the same place, or has nowhere to go but the end of the block.
StatementEdges {
    // expect: poisoned
    consecutive { a = ; ; b = ; ^1 }
    // expect: poisoned
    consecutive_at_end { 1 + ; 2 + ; 3 + }
    // expect: poisoned
    only_error { + }
    // expect: poisoned
    in_immediately_evaluated { (var a = 1; 1 + ; ^a) }
    // expect: poisoned
    in_closure { ^{ |x| x + ; x * 2 }.value(3) }
    // expect: poisoned
    in_call_function { ^[1, 2].collect { |x| x + ; x } }
    // expect: poisoned
    in_nested_functions { { { 1 + ; ^1 }.value ; ^2 }.value ; ^3 }
    // expect: poisoned
    non_local_return { if (true) { 1 + ; ^1 }; if (false) { ^2 }; ^3 }
    // expect: poisoned
    primitive { _Prim ; 1 + ; ^1 }
    // expect: poisoned
    *class_method { 1 + ; ^1 }
    // expect: -> 8
    good { ^8 }
}
