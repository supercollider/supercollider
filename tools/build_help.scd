SCDoc.helpSourceDir_("SC_HELP_SOURCE_DIR".getenv);

SCDoc.helpTargetDir_("SC_HELP_TARGET_DIR".getenv.mkdir);

SCDoc.renderAll(includeExtensions: false);

if(SCDoc.parseErrors.size > 0, {
	"Error: Some help documents could not be parsed!".postln;
	SCDoc.parseErrors.flatten(numLevels: 1).join("\n").postln;
	1.exit;
});

0.exit;
