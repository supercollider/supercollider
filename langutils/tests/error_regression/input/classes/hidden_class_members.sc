// compiled
// A subclass's class variable or constant can hide its superclass's, as the old compiler allowed: this class and its
// subclasses use their own, the superclass's methods use theirs. Legacy says nothing, see hidden_class_members_316.sc.
// An instance variable needs its own slot, so that is still an error, see duplicate_members.sc.
HiddenBase {
    classvar <delim = $ ;
    const nl = "\n";
    *baseDelim { ^delim }
    *baseNl { ^nl }
}

HiddenSub : HiddenBase {
    classvar <delim = $,;
    const <nl = $\n;
    *subDelim { ^delim }
    *subNl { ^nl }
    *setDelim { |d| delim = d; ^d }
}

HiddenSubSub : HiddenSub {
    *subSubDelim { ^delim }
}

HiddenInstanceBase {
    var a;
}

HiddenInstance : HiddenInstanceBase {
    var a;
}
