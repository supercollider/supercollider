//# VERSION 3.16
// The indentation is only used when it explains the unclosed brackets without more errors. Here `unindented`'s body
// isn't indented, so going by the indentation would close it early and leave its `}` unmatched, so the brackets are
// paired as written, and the class's `{` is reported.
UnclosedByIndentationFallback {
    unindented {
    ^1
    }
    missing_close {
        ^2
}
