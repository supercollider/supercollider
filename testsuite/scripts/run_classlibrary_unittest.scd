try {
    UnitTest.runAll { |durations, passes, failures, knownIssues, success| 
        UnitTestResultsPrinter(durations, passes, failures, knownIssues, success);
        if(success) { 0.exit } { 1.exit }
    }
} { |er|
    er.reportError;
    2.exit
}
