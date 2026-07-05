TestQuark : UnitTest {
	classvar <>testResults;

	*knownIssues {
		^(
			\test_ensureQuarkAssetExists: UnitTestKnownIssue(
				"https://github.com/supercollider/supercollider/issues/7611", 
				when: { |platform| platform.name == \windows } 
			),

			\test_runQuarkInstallHooks: UnitTestKnownIssue(
				"https://github.com/supercollider/supercollider/issues/7610",
				when: { |platform| platform.name == \linux }
			)
		)
	}

	*initClass { testResults = () }

	*testQuarkPath {
		^PathName(thisMethod.filenameSymbol.asString).pathOnly +/+ "assets" +/+ "TestQuark";
	}

	*tempDir {
		^Platform.defaultTempDir +/+ 'testTempDir' +/+ 'testQuark';
	}

	*clearTempDir {
		File.delete(TestQuark.tempDir);
	}

	*hookNames {
		^[
			\preInstall,
			\postInstall,
			\preUpdate,
			\postUpdate,
			\preUninstall,
			\postUninstall,
		];
	}

	test_ensureQuarkAssetExists {
		var thisFilePath, theseEntries, assetsDir, testQuarkDir, testQuarkFile;
		this.assert(thisMethod.filenameSymbol.isKindOf(Symbol), "thisMethod.filenameSymbol should be a symbol");
		this.assert(thisMethod.filenameSymbol.asString.size != 0, "thisMethod.filenameSymbol should not be empty");

		thisFilePath = PathName(thisMethod.filenameSymbol.asString);
		this.assert(thisFilePath.isFile, "file should exist");

		theseEntries = PathName(thisFilePath.pathOnly).entries;

		assetsDir = theseEntries.select { |p| p.isFolder and: {p.folderName == "assets" }}[0];

		this.assert(assetsDir.isKindOf(PathName), "assets directory should exist");

		testQuarkDir = assetsDir.entries.select { |p| p.isFolder and: {p.folderName == "TestQuark"} }[0];

		this.assert(testQuarkDir.isKindOf(PathName), "test quark dir inside assets dir should exist");

		testQuarkFile = testQuarkDir.entries.select { |f| f.isFile and: {f.fileName == "TestQuark.quark"}}[0];

		this.assert(testQuarkFile.isKindOf(PathName), "test quark file should exist");

		this.assertEquals(TestQuark.testQuarkPath +/+ "TestQuark.quark", testQuarkFile.absolutePath, "test quark path class method should equal the constructed path");
	}


    test_parseQuarkFileHooksAsFunction {
        var quark = Quark.fromLocalPath(TestQuark.testQuarkPath);
		TestQuark.hookNames.do({|hookName|
			this.assert(
				quark.data[hookName].class == Function,
				"Hook '%' should be parsed as function".format(hookName),
			);
		});
    }

	test_runQuarkInstallHooks {
		// as the quark to be installed is empty we can not update or
		// uninstall it.
		// Making it non empty does not fix this as it is already
		// in a subdir of the testsuite, therefore every class file
		// will be already available and a proper Quark install
		// is omitted.
		try { Quarks.install(TestQuark.testQuarkPath)} { |er|
			this.assert(false, "could not install quark" ++ er.what);
			er.throw;
		};

		[\preInstall, \postInstall].do({|hookName|
			this.assert(
				testResults[hookName] == true,
				"Hook '%' should have been called".format(hookName);
			);
		});

		try { TestQuark.clearTempDir } { |er|
			this.assert(false, "could not clear temp dir");
			er.throw
		};

		testResults = ();
	}

	test_parseQuarkFileData {
		var quark = Quark.fromLocalPath(TestQuark.testQuarkPath);
		this.assert(
			quark.data[\name] == "DemonWidgets",
			"Quark name should be parsed from quark file",
		);
		this.assert(
			quark.data[\version] == "1.0.0",
			"Quark version should be parsed from quark file"
		);
	}
}
