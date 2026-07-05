TestLanguageConfig : UnitTest {
	test_ensureAllAreFolders {
		this.assert(LanguageConfig.includePaths.every { |p| PathName(p).isFolder });
	}

	test_ensureAllExist {
		this.assert(LanguageConfig.includePaths.every { |p| p.pathMatch.size > 0 });
	}
}
