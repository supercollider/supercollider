TestSCDoc : UnitTest {

    test_helpSourceDirs_excludedExtensions {
        var result;
        var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
        var prevSetting = LanguageConfig.excludeDefaultPaths;
        LanguageConfig.excludeDefaultPaths = true;
        result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
        LanguageConfig.excludeDefaultPaths = prevSetting;
        this.assert(result.not, "should not search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = true, extensionPaths: %, helpSourcePaths: %".format(extensionPaths, SCDoc.helpSourceDirs));
    }

    test_helpSourceDirs_includedExtensions {
        var result;
        var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
        var prevSetting = LanguageConfig.excludeDefaultPaths;
        LanguageConfig.excludeDefaultPaths = false;
        result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
        LanguageConfig.excludeDefaultPaths = prevSetting;
        this.assert(result, "should search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = false, extensionPaths: %, helpSourcePaths: %".format(extensionPaths, SCDoc.helpSourceDirs));
    }

}
