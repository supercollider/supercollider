//# VERSION 3.16
// A method that is never closed, after an error in a statement.
Unterminated {
    // expect: -> 1
    before { ^1 }
    broken { 1 + ; ^2
