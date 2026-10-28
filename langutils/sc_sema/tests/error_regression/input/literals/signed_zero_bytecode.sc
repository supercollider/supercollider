// bytecode
//# VERSION 3.16
// From 3.16 `-0.0` is its own constant, so it is in the constants table, while `0.0` is still PushSpecialNumber. Each
// method calls `touch` first so it isn't replaced by a shortcut.
SignedZeroBytecode {
    touch { }
    positive { this.touch; ^0.0 }
    negative { this.touch; ^-0.0 }
    both { this.touch; ^[0.0, -0.0] }
}
