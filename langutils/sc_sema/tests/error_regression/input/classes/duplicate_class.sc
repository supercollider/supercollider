// module: duplicate_class
//# VERSION 3.16
// A class defined twice is an error, and the first definition is used, by module, then file, then position, so it is
// the same one every time, however many threads declare the classes. This is the duplicate, the earlier module's is
// compiled.
InEarlierModule {
    which { ^\later }
}

DefinedTwice {
    // expect: no diagnostics
    // expect: -> \first
    which { ^\first }
}

DefinedTwice {
    which { ^\second }
}
