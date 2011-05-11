SCDoc.helpSourceDir = thisProcess.argv[0];
SCDoc.helpTargetDir = thisProcess.argv[1];
~exitOnDone = {0.exit};
SCDoc.renderAll(doneFunc: ~exitOnDone, threaded: false, findExtensions: false);
