// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = true;

	// alphabets used in testing
	classvar alphanumAlphabet; // alphanum + [.+-(pi)]
	classvar numericalAlphabet; // num + [eE.+-(pi)]
	classvar accidentalAlphabet; // num + [sb+-]

	// maximum string size - we start from strlen=0 for each.
	// There will be lots of duplication, but since
	// these alphabets are small, it's not a big deal.
	// Besides, it allows us to decouple them if desired.
	classvar alphanumAlphabetStringSizeLimit = 3;
	classvar numericalAlphabetStringSizeLimit = 5;
	classvar accidentalAlphabetStringSizeLimit = 6;

	classvar directory = "brutal_lexer_targeted_results/";

	*new {
		^super.new.initAlphabets;
	}

	initAlphabets {
		// init alphabets
		alphanumAlphabet = (1..127).collect(_.asAscii).select(_.isAlphaNum);
		alphanumAlphabet = alphanumAlphabet.collect(_.asString);
		alphanumAlphabet = alphanumAlphabet ++ ["pi", "+", "-", "."];

		// numericalAlphabet = (1..127).collect(_.asAscii).select(_.isDecDigit);
		// numericalAlphabet = numericalAlphabet.collect(_.asString);
		// numericalAlphabet = numericalAlphabet ++ ["pi", "+", "-", ".", "e", "E"];
		// don't use all digits—this is really just testing the limits of
		// integer and floating-point lexing
		numericalAlphabet = ["0", "1", "9", "pi", "+", "-", ".", "e"];

		// accidentalAlphabet = (1..127).collect(_.asAscii).select(_.isDecDigit);
		// accidentalAlphabet = accidentalAlphabet.collect(_.asString);
		// accidentalAlphabet = accidentalAlphabet ++ ["s", "b"];
		// again, don't need a complete set for this
		accidentalAlphabet = ["0", "1", "9", "s", "b"];
	}

	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		} {
			postln("TestLexerTargetedBrutal: no diffs found.");
		}
	}

	runLexerTestsOnAlphabet {
		arg min, max, alphabet, prefix, suffix, filenameFormat, alphabetName;

		var diffs;

		(min..max).do {
			arg len;

			diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				alphabet,
				len,
				prefix,
				suffix,
				filenameFormat.format(alphabetName, len),
				\compile,
				makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};
	}

	// TODO: refactor these as one method with a symbol switch
	runAlphanumLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runLexerTestsOnAlphabet(
			0, alphanumAlphabetStringSizeLimit,
			alphanumAlphabet, prefix, suffix, filenameFormat, "alnum"
		);
	}

	runNumericalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runLexerTestsOnAlphabet(
			0, numericalAlphabetStringSizeLimit,
			numericalAlphabet, prefix, suffix, filenameFormat, "num"
		);
	}

	runAccidentalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runLexerTestsOnAlphabet(
			0, accidentalAlphabetStringSizeLimit,
			accidentalAlphabet, prefix, suffix, filenameFormat, "acc"
		);
	}

}