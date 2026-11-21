// A version comment below anything else, even a comment, isn't the file's, so it is misplaced and its classes are
// legacy.
//# VERSION 3.16
NotFirstLine {
    // expect-error: Variable declared after an expression
    one { 1; var b = 2; ^b }
}
