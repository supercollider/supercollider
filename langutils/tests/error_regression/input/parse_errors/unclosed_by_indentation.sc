//# VERSION 3.16
// When brackets are left open at the end of the file, the indentation is used to guess which one wasn't closed: a `{`
// ending its line is closed before the next line indented no further than the line it is on.
UnclosedByIndentation {
    // expect: -> 1
    before { ^1 }
    // expect: poisoned
    // expect-error: Unclosed Bracket.
    missing_close {
        ^2

    // expect: -> 3
    after { ^3 }
    // expect: poisoned
    // expect-error: Unclosed Bracket.
    missing_nested_close { |x|
        if (x) {
            ^4
        // The method's own `}` is next.
    }
    // expect: -> 5
    last { ^5 }
}
