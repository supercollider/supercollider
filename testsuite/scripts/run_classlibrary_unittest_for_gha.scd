// Can't open a window on github action (it just hangs), the cli environment doesn't support the Document class, and midi doesn't work on gha either
try {
    UnitTest.runFiltered { |u| 
        u.needsOpenWindow.not and: {u.needsDocument.not} and: {u.needsMIDI.not} 
    } { |durations, passes, failures, knownIssues, success| 
        UnitTestResultsPrinter(durations, passes, failures, knownIssues, success, options: (
            \supressPassedResults: true,
            \skipReason: "Disabling all tests that cannot be checked on GHA. On GitHub you cannot open a window or use MIDI, and in a CLI environment, there is no open document."
        ));
        if(success) { 0.exit } { 1.exit }
    }
} { |er|
    er.reportError;
    2.exit;
}
