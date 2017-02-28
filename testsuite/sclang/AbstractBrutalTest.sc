// AbstractBrutalTest.sc
// Brian Heim, 2017-02-28

AbstractBrutalTest : UnitTest {
	// alphabets and associated data used in testing

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	var <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	var <>alphabetStringLengths;

	*new {
		^super.new.initAlphabets;
	}

	// this method fills `alphabets` and `maxLengths`
	initAlphabets { ^this.subclassResponsibility(thisMethod) }

	// used to determine the output location
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// should return `true` if you want to generate validation files,
	// and return `false` for ordinary testing
	makingValidationFiles { ^this.subclassResponsibility(thisMethod) }

	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		} {
			postf("%: No diffs found.\n".format(this.class.name));
		}
	}

	runTestsOnAlphabet {
		arg prefix, suffix, testMode, alphabetName, technique;

		this.printTestMode(testMode);
		this.createOutputDir();

		alphabetStringLengths[alphabetName].do {
			arg len;

			var diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				alphabets[alphabetName],
				len,
				prefix,
				suffix,
				filenameFormat.format(alphabetName, len),
				technique,
				this.makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};
	}

	printTestMode {
		arg mode;

		"".postln;
		"%: running test mode %".format(this.class, mode).underlined.postln;
	}

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
