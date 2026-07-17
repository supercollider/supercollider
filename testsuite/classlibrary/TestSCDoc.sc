TestSCDoc : UnitTest {

	test_helpSourceDirs_excludedExtensions {
		var result;
		var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
		var prevSetting = LanguageConfig.excludeDefaultPaths;
		var tmpHelpSourcePath = Platform.userExtensionDir +/+ "HelpSource";
		var folderWasCreated = false;
		LanguageConfig.excludeDefaultPaths = true;
		if(File.exists(tmpHelpSourcePath).not) {
			// SCDoc.prRescanHelpSourceDirs will only report entries
			// in userExtensionsDir, if it contains a HelpSource subfolder
			// we create one just for this test
			File.mkdir(tmpHelpSourcePath);
			folderWasCreated = true;
		};
		result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
		LanguageConfig.excludeDefaultPaths = prevSetting;
		if(folderWasCreated) { File.delete(tmpHelpSourcePath) };
		this.assert(result.not, "should not search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = true");
	}

	test_helpSourceDirs_includedExtensions {
		var result;
		var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
		var prevSetting = LanguageConfig.excludeDefaultPaths;
		var tmpHelpSourcePath = Platform.userExtensionDir +/+ "HelpSource";
		var folderWasCreated = false;
		LanguageConfig.excludeDefaultPaths = false;
		if(File.exists(tmpHelpSourcePath).not) {
			// SCDoc.prRescanHelpSourceDirs will only report entries
			// in userExtensionsDir, if it contains a HelpSource subfolder
			// we create one just for this test
			File.mkdir(tmpHelpSourcePath);
			folderWasCreated = true;
		};
		result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
		LanguageConfig.excludeDefaultPaths = prevSetting;
		if(folderWasCreated) { File.delete(tmpHelpSourcePath) };
		this.assert(result, "should search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = false");
	}

}
