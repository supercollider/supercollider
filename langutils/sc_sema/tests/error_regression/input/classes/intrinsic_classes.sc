// intrinsic: IntrinsicGood : Object [slot] a b
// intrinsic: IntrinsicSub : IntrinsicGood c
// intrinsic: IntrinsicWrongSuper : Object
// intrinsic: IntrinsicWrongSlot : Object [char]
// intrinsic: IntrinsicMissingSlot : Object [slot]
// intrinsic: IntrinsicUnexpectedSlot : Object
// intrinsic: IntrinsicTooMany : Object a
// intrinsic: IntrinsicTooFew : Object a b
// intrinsic: IntrinsicNone : Object a
// intrinsic: IntrinsicUnexpected : Object
// intrinsic: IntrinsicWrongName : Object a b
// intrinsic: IntrinsicValue : Object a
// The VM relies on an intrinsic class's superclass, slot type and own instance variables, those it doesn't inherit,
// so the class library must declare them as the VM does, see sc_inspect's --intrinsic. Their initial values are the
// class library's.
IntrinsicGood[slot] { var a, b; }
IntrinsicSub : IntrinsicGood { var c; }
IntrinsicBase {}
IntrinsicWrongSuper : IntrinsicBase {}
IntrinsicWrongSlot[slot] {}
IntrinsicMissingSlot {}
IntrinsicUnexpectedSlot[slot] {}
IntrinsicTooMany { var a, b; }
IntrinsicTooFew { var a; }
IntrinsicNone {}
IntrinsicUnexpected { var a; }
IntrinsicWrongName { var a, c; }
IntrinsicValue { var a = 1; }
