//# VERSION 3.16
// A class whose superclass isn't defined is poisoned while declaring the classes. Only that is reported: its methods,
// and those of its subclasses, aren't compiled, so its undefined members aren't reported too.
Orphan : Undefined {
    var member = 1;
    classvar class_var;

    reads_member { ^member }
    reads_undefined { ^not_defined }
    *class_method { ^class_var }
}

OrphanChild : Orphan {
    child_method { ^member }
}

SecondOrphan : Undefined {
    method { ^1 }
}

Fine {
    method { ^1 }
}
