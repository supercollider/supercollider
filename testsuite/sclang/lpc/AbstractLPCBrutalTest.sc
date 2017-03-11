// AbstractLPCBrutalTest.sc (formerly AbstractBrutalTest.sc)
// Brian Heim, 2017-02-28

AbstractLPCBrutalTest : UnitTest {
	// alphabets and associated data used in testing

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	classvar <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	classvar <>stringLengthsPerAlphabet;

	// List of files that contain diffs to be printed when cleaning up.
	classvar <>diffFilenames;

	*new { ^super.new.initTestConditions; }

	initTestConditions {
		alphabets = this.getAlphabets();
		stringLengthsPerAlphabet = this.getStringLengthsPerAlphabet();
	}

	getAlphabets { ^this.subclassResponsibility(thisMethod) }

	getStringLengthsPerAlphabet { ^this.subclassResponsibility(thisMethod) }

	evaluationTechnique { ^this.subclassResponsibility(thisMethod) }

	// Output directory relative to working path. Will be created if
	// it doesn't already exist.
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// Should return `true` if you want to generate validation files,
	// and return `false` for ordinary testing.
	makingValidationFiles { ^this.subclassResponsibility(thisMethod) }

	// For a given alphabet, test name, prefix, and suffix, run all the tests on that
	// alphabet for its requested string lengths. This method also constructs uniform filenames.
	runTestsOnAlphabet {
		arg prefix, suffix, testMode, alphabetName;

		var fullOutputDir = this.outputDir.resolveRelative;

		this.printTestMode(testMode);
		this.createOutputDir();

		stringLengthsPerAlphabet[alphabetName].do {
			arg len;

			var filename = "%%_%_%".format(fullOutputDir, alphabetName, len, testMode);

			if(this.makingValidationFiles) {
				filename = filename ++ LPCTestUtils.validatedOutputFilenameSuffix;
			};

			LPCTestUtils.evaluateAllStrings(
				  alphabet: alphabets[alphabetName],
				       len: len,
				    prefix: prefix,
				    suffix: suffix,
				  filename: filename,
				 technique: this.evaluationTechnique,
				  compress: true
			);

			// redundant under the current implementation of evaluateAllStrings
			/*if(diffs.isNil) {
				Error("%: diffs should not be nil".format(thisMethod)).throw;
			};*/

			if(this.makingValidationFiles.not) {
				var diffs = LPCTestUtils.compareFiles(filename);
				File.delete(filename);
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
			postf("No diffs found.\n");
		}
	}

	// Give a nicely formatting string to inform the user what test mode we started.
	printTestMode {
		arg mode;

		"".postln;
		"Running test mode %".format(mode.quote).underlined.postln;
	}

	// Force creation of this test's output directory
	createOutputDir {
		var dirname = this.outputDir.resolveRelative;

		if(File.exists(dirname).not) {
			"%: Creating directory %".format(thisMethod, dirname.quote).postln;
			try {
				File.mkdir(dirname);
			} {
				Error("%: Could not create directory %".format(thisMethod, this.outputDir.quote)).throw;
			}
		}
	}

	// Note failures so that diff filenames can be printed at the end of the run.
	noteFailure {
		arg method, message, filename;

		this.failed(method, message);

		diffFilenames = diffFilenames !? _.add(filename) ? [filename];
	}

	// Wrap .run to allow overriding of alphabet string lengths
	*run {
		arg reset = true, report = true, strlensPerAlphabet = nil;

		this.stringLengthsPerAlphabet_(strlensPerAlphabet);

		super.run(reset, report);
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
