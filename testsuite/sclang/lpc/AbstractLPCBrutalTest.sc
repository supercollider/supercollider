// AbstractLPCBrutalTest.sc (formerly AbstractBrutalTest.sc)
// Brian Heim, 2017-02-28

AbstractLPCBrutalTest : UnitTest {
	// alphabets and associated data used in testing

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	var <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	var <>stringLengthsPerAlphabet;

	// List of files that contain diffs to be printed when cleaning up.
	classvar <>diffFilenames;

	*new { ^super.new.initAlphabets.initStringLengthsPerAlphabet; }

	// Inits `alphabets`.
	initAlphabets { ^this.subclassResponsibility(thisMethod) }

	// Inits `stringLengthsPerAlphabet`
	initStringLengthsPerAlphabet { ^this.subclassResponsibility(thisMethod) }

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

		stringLengthsPerAlphabet[alphabetName].do {
			arg len;

			var filename = "%%_%_%".format(this.outputDir, alphabetName, len, testMode);

			var diffs = LPCTestUtils.evaluateAllStrings(
				  alphabet: alphabets[alphabetName],
				       len: len,
				    prefix: prefix,
				    suffix: suffix,
				    testID: filename,
				 technique: technique,
				doValidate: this.makingValidationFiles,
				  compress: true
			);

			// redundant under the current implementation of evaluateAllStrings
			/*if(diffs.isNil) {
				Error("%:%: diffs should not be nil".format(this.class, thisMethod)).throw;
			};*/

			if(this.makingValidationFiles.not) {
				this.checkDiffs(diffs, filename);
			}
		};
	}

	// Respond to the diffs. If there are any, fail now and print a message.
	// Otherwise, note that no diffs were found.
	checkDiffs {
		arg diffs, filenameBase;

		if(diffs.isEmpty.not) {
			var diffFilename = filenameBase ++ LPCTestUtils.diffOutputFilenameSuffix;

			LPCTestUtils.writeDiffs(diffs, diffFilename);

			this.noteFailure(thisMethod, "Diffs were found between test and validation files", diffFilename);
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

	// Note failures so that diff filenames can be printed at the end of the run.
	noteFailure {
		arg method, message, filename;

		this.failed(method, message);

		diffFilenames = diffFilenames !? _.add(filename) ? [filename];
	}

	// Wrap .report to print a list of filenames when done
	*report {
		arg ... args;

		super.report(*args);

		if(diffFilenames.notNil) {
			"\nSome validation tests did not pass.\nThe following output files were generated:\n".postln;

			diffFilenames.do {
				arg filename;
				"\t%\n".postf(filename);
			}
		}
	}

}
