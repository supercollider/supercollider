// AbstractLPCBrutalTest.sc (formerly AbstractBrutalTest.sc)
// Brian Heim, 2017-02-28

AbstractLPCBrutalTest : UnitTest {
	const <correctSuffix = "_correct";
	const <diffSuffix = "_diff";

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	classvar <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	classvar <>stringLengthsPerAlphabet;

	// Filenames to be printed in final report.
	classvar <>diffFilenames;

	/**** ABSTRACT METHODS ****/

	getAlphabets { ^this.subclassResponsibility(thisMethod) }

	getStringLengthsPerAlphabet { ^this.subclassResponsibility(thisMethod) }

	evaluationTechnique { ^this.subclassResponsibility(thisMethod) }

	// Relative to working path. Created if missing.
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// Should return `false` for testing.
	makingValidationFiles { ^this.subclassResponsibility(thisMethod) }

	/**** IMPLEMENTED METHODS ****/

	*new { ^super.new.initTestConditions; }

	initTestConditions {
		alphabets = this.getAlphabets();
		stringLengthsPerAlphabet = this.getStringLengthsPerAlphabet();
	}

	// For a given alphabet, test name, prefix, and suffix, run all the tests on that
	// alphabet for its requested string lengths.
	runTestsOnAlphabet {
		arg prefix, suffix, testMode, alphabetName;

		var fullOutputDir = this.outputDir.resolveRelative;

		this.printTestMode(testMode);
		this.createOutputDir();

		stringLengthsPerAlphabet[alphabetName].do {
			arg len;
			var filename = "%%_%_%".format(fullOutputDir, alphabetName, len, testMode);

			if(this.makingValidationFiles) { filename = filename++correctSuffix };

			LPCTestUtils.evaluateAllStrings(
				  alphabet: alphabets[alphabetName],
				       len: len,
				    prefix: prefix,
				    suffix: suffix,
				  filename: filename,
				 technique: this.evaluationTechnique,
				  compress: true
			);

			if(this.makingValidationFiles.not) {
				var diffs = LPCTestUtils.compareFiles(filename, filename++correctSuffix);

				this.checkDiffs(diffs, filename);
				File.delete(filename);
			}
		}
	}

	// If there were diffs, fail now and print a message.
	checkDiffs {
		arg diffs, filenameBase;

		if(diffs.isEmpty.not) {
			var diffFilename = filenameBase ++ diffSuffix;

			LPCTestUtils.writeDiffs(diffs, diffFilename);
			this.noteFailure(thisMethod, "Diffs were found between test and validation files", diffFilename);
		} {
			postf("No diffs found.\n");
		}
	}

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

	/**** Overloaded methods from UnitTest ****/

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
			diffFilenames.do("\t%\n".postf(_));
		}
	}

}
