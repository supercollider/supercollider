SCDoc.helpSourceDir = thisProcess.argv[0];
SCDoc.helpTargetDir = thisProcess.argv[1];
~exitOnDone = {0.exit};
SCDoc.updateAll(doneFunc: ~exitOnDone, threaded: false, gui: false, findExtensions: false, useHelpBase: false);
