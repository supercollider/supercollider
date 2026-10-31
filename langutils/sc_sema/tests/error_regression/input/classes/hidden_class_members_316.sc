// compiled
//# VERSION 3.16
// From 3.16 a subclass hiding its superclass's class variable or constant is a warning, as it is easily done by
// mistake, see hidden_class_members.sc.
// An instance variable needs its own slot, so that is still an error, see duplicate_members.sc.
Hidden316Base {
    classvar <delim = $ ;
    const nl = "\n";
    *baseDelim { ^delim }
    *baseNl { ^nl }
}

Hidden316Sub : Hidden316Base {
    classvar <delim = $,;
    const <nl = $\n;
    *subDelim { ^delim }
    *subNl { ^nl }
    *setDelim { |d| delim = d; ^d }
}

Hidden316SubSub : Hidden316Sub {
    *subSubDelim { ^delim }
}

Hidden316InstanceBase {
    var a;
}

Hidden316Instance : Hidden316InstanceBase {
    var a;
}
