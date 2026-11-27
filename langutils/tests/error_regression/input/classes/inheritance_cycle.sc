//# VERSION 3.16
// Classes in an inheritance cycle, or under a superclass that isn't defined, are poisoned, so aren't compiled. The
// diagnostics, and the order of the classes in them, are the same every time, by name.
CycleB : CycleA {}
CycleA : CycleB {}

ItsOwnSuper : ItsOwnSuper {}

ChildZ : NotDefined {}
ChildY : NotDefined {}

Fine {
    // expect: no diagnostics
    // expect: -> 1
    one { ^1 }
}
