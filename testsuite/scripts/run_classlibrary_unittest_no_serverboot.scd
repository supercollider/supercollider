try { 
    UnitTest.runFiltered { |u| 
        u.needsServerBoot.not and: {u.needsDocument.not} 
    } { |durations, passes, failures, knownIssues, success| 
        UnitTestResultsPrinter.printOnto(Post, durations, passes, failures, knownIssues, success, (printSimple: true));
        if(success) { 0.exit } { 1.exit }
    }
} { |er|
    er.reportError;
    2.exit
}
