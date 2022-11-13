TestQuark : UnitTest {
	classvar <>testResults;

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
		TestQuark.clearTempDir;
		// as the quark to be installed is empty we can not update or
		// uninstall it.
		// Making it non empty does not fix this as it is already
		// in a subdir of the testsuite, therefore every class file
		// will be already available and a proper Quark install
		// is omitted.
		Quarks.install(TestQuark.testQuarkPath);
		[\preInstall, \postInstall].do({|hookName|
			this.assert(
				testResults[hookName] == true,
				"Hook '%' should have been called".format(hookName);
			);
		});
		TestQuark.clearTempDir;
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
