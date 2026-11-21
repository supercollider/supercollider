//# VERSION 3.16
// A class with an invalid slot type is poisoned while declaring the classes, and so are the classes inheriting from
// it. Only the invalid slot type is reported.
BadSlot[bad] {
    var member;
    method { ^member }
}

ChildOfBadSlot : BadSlot {
    child_method { ^undefined }
}

GrandchildOfBadSlot : ChildOfBadSlot {
    *class_method { ^undefined }
}

FloatSlot[float] {
    // expect: no diagnostics
    method { ^1 }
}
