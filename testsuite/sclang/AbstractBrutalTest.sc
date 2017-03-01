// AbstractBrutalTest.sc
// Brian Heim, 2017-02-28

AbstractBrutalTest : UnitTest {
	// alphabets and associated data used in testing

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	var <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	var <>alphabetStringLengths;

	*new { ^super.new.initAlphabets; }

	// Inits `alphabets` and `maxLengths`.
	initAlphabets { ^this.subclassResponsibility(thisMethod) }

	// Used to determine the output location.
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// Should return `true` if you want to generate validation files,
	// and return `false` for ordinary testing.
	makingValidationFiles { ^this.subclassResponsibility(thisMethod) }

	// For a given alphabet, technique, test name, prefix, and suffix,
	// run all the tests on that alphabet for its requested string lengths.
	// This method also constructs uniform filenames.
	runTestsOnAlphabet {
		arg prefix, suffix, testMode, alphabetName, technique;

		this.printTestMode(testMode);
		this.createOutputDir();

		alphabetStringLengths[alphabetName].do {
			arg len;

			var diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				  alphabet: alphabets[alphabetName],
				       len: len,
				    prefix: prefix,
				    suffix: suffix,
				    testID: "%%_%_%".format(this.outputDir, alphabetName, len, testMode),
				 technique: technique,
				doValidate: this.makingValidationFiles,
				  compress: true
			);

			if(this.makingValidationFiles.not) {
				this.checkDiffs(diffs);
			}
		};
	}

	// Respond to the diffs. If there are any, fail now and print a message.
	// Otherwise, note that no diffs were found.
	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		} {
			postf("%: No diffs found.\n".format(this.class.name));
		}
	}

	// Give a nicely formatting string to inform the user what test mode we started.
	printTestMode {
		arg mode;

		"".postln;
		"%: running test mode %".format(this.class, mode.quote).underlined.postln;
	}

	// Force creation of this test's output directory
	createOutputDir {
		var dirname = this.outputDir.resolveRelative;

		if(File.exists(dirname).not) {
			"%: Creating directory %".format(this.class, dirname.quote).postln;
			try {
				File.mkdir(dirname);
			} {
				Error("%: Could not create directory %".format(this.class, this.outputDir.quote)).throw;
			}
		}
	}

}
