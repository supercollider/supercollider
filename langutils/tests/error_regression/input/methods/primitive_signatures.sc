// primitive: _PrimOne 2
// primitive: _PrimArgs 1 args
// primitive: _PrimKwargs 1 args kwargs
// Only the VM knows its primitives' arguments, counting the receiver, and whether they take `...args` and kwargs, see
// sc_inspect's --primitive. A method calling one must match, as the old compiler checked. One the VM doesn't define
// isn't checked.
PrimitiveSignatures {
    // expect: no diagnostics
    one { |a| _PrimOne ^this.primitiveFailed }
    // expect: poisoned
    // expect-error: Primitive argument mismatch
    oneMissing { _PrimOne ^this.primitiveFailed }
    // expect: poisoned
    // expect-error: Primitive argument mismatch
    oneExtra { |a, b| _PrimOne ^this.primitiveFailed }
    // expect: no diagnostics
    args { |...rest| _PrimArgs ^this.primitiveFailed }
    // expect: poisoned
    // expect-error: Primitive argument mismatch
    argsMissing { _PrimArgs ^this.primitiveFailed }
    // expect: no diagnostics
    kwargs { |...rest, kwargs| _PrimKwargs ^this.primitiveFailed }
    // expect: poisoned
    // expect-error: Primitive argument mismatch
    kwargsMissing { |...rest| _PrimKwargs ^this.primitiveFailed }
    // expect: no diagnostics
    unknown { |a, b, c| _NotInTheTable ^this.primitiveFailed }
}
