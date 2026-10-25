// bytecode
// Fixed: a unary operator past the first 16 written as a binary uses SendSpecialUnaryArithMsgX. The old compiler always
// used the short form, so its Debug build asserts here and its Release build overflowed into SendSpecialBinaryArithMsg
// (`a + b`). As old sclang can't compile it, differential_test.py skips this file.
BytecodeUnaryOverflow {
    classvar cv;

    lastShort { |a, b| cv; ^a exp: b }
    firstLong { |a, b| cv; ^a reciprocal: b }
}
