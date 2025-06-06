SCDoc.helpSourceDir_("SC_HELP_SOURCE_DIR".getenv);

SCDoc.helpTargetDir_("SC_HELP_TARGET_DIR".getenv.mkdir);

SCDoc.renderAll(includeExtensions: false);

0.exit;
