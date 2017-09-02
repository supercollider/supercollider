// AbstractLPCBrutalTest.sc (formerly AbstractBrutalTest.sc)
// Brian Heim, 2017-02-28

AbstractLPCBrutalTest : UnitTest {
	const <expectedDir = "expected";
	const <actualDir   = "actual";
	const <diffDir     = "diff";

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	var <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	var <>stringLengthsPerAlphabet;

	// Filenames to be printed in final report.
	classvar <>diffFilenames;

	classvar <>deleteActualFilesOnFinish = false;

	/**** ABSTRACT METHODS ****/

	getAlphabets { ^this.subclassResponsibility(thisMethod) }

	getStringLengthsPerAlphabet { ^this.subclassResponsibility(thisMethod) }

	evaluationTechnique { ^this.subclassResponsibility(thisMethod) }

	// Relative to working path. Created if missing.
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// Should return `true` for testing.
	performingValidation { ^this.subclassResponsibility(thisMethod) }

	/**** IMPLEMENTED METHODS ****/

	*new { ^super.new.initTestConditions; }

	initTestConditions {
		alphabets = this.getAlphabets();
		stringLengthsPerAlphabet = this.getStringLengthsPerAlphabet();
	}

	getExpectedDir { ^expectedDir.resolveRelative +/+ this.outputDir +/+ "" }
	getActualDir   { ^actualDir.resolveRelative   +/+ this.outputDir +/+ "" }
	getDiffDir     { ^diffDir.resolveRelative     +/+ this.outputDir +/+ "" }

	runTestsTogglingTCO {
		arg prefix, suffix, testMode, alphabetName;

		var tco = Process.tailCallOptimize;

		Process.tailCallOptimize_(true);
		this.runTestsOnAlphabet(prefix, suffix, testMode++"TCO", alphabetName);

		Process.tailCallOptimize_(false);
		this.runTestsOnAlphabet(prefix, suffix, testMode++"NoTCO", alphabetName);

		Process.tailCallOptimize_(tco);
	}

	// For a given alphabet, test name, prefix, and suffix, run all the tests on that
	// alphabet for its requested string lengths.
	runTestsOnAlphabet {
		arg prefix, suffix, testMode, alphabetName;

		var fullOutputDir = this.outputDir.resolveRelative;

		this.printTestMode(testMode, alphabetName);
		LPCTestUtils.safeMkdir(this.getActualDir);

		stringLengthsPerAlphabet[alphabetName].do {
			arg len;
			var filename = "%_%_%".format(alphabetName, len, testMode);

			LPCTestUtils.evaluateAllStrings(
				  alphabet: alphabets[alphabetName],
				       len: len,
				    prefix: prefix,
				    suffix: suffix,
				  filename: this.getActualDir +/+ filename,
				 technique: this.evaluationTechnique,
				  compress: true
			);

			if(this.performingValidation) {
				var diffs = LPCTestUtils.compareFiles(
					this.getActualDir +/+ filename,
					this.getExpectedDir +/+ filename
				);

				this.handleDiffs(diffs, filename);
			};

			if(deleteActualFilesOnFinish) {
				File.delete(this.getActualDir +/+ filename);
			}
		}
	}

	// If there were diffs, fail now and print a message.
	handleDiffs {
		arg diffs, filename;

		if(diffs.isEmpty.not) {
			LPCTestUtils.safeMkdir(this.getDiffDir);
			LPCTestUtils.writeDiffs(diffs, this.getDiffDir +/+ filename);
			this.noteFailure(thisMethod, "Diffs were found between test and validation files", filename);
		} {
			postf("No diffs found.\n");
		}
	}

	printTestMode {
		arg mode, alphName;
		"".postln;
		"Running test mode % on alphabet %".format(mode.quote, alphName.asString.quote).underlined.postln;
	}

	// Note failures so that diff filenames can be printed at the end of the run.
	noteFailure {
		arg method, message, filename;
		this.failed(method, message);
		diffFilenames = diffFilenames !? _.add(filename) ? [filename];
	}

	/**** Overloaded methods from UnitTest ****/

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
