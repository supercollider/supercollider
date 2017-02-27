// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = true;

	// alphabets used in testing
	classvar numericalAlphabet; // alphanum + (pi) + [.+-]
	classvar accidentalAlphabet; // num + [sb+-]

	// maximum string size - we start from strlen=0 for each.
	// There will be lots of duplication, but since
	// these alphabets are small, it's not a big deal.
	// Besides, it allows us to decouple them if desired.
	classvar numericalAlphabetStringSizeLimit = 3;
	classvar accidentalAlphabetStringSizeLimit = 6;

	classvar directory = "brutal_lexer_targeted_results/";

	*new {
		^super.new.initAlphabets;
	}

	initAlphabets {
		// init alphabets
		numericalAlphabet = (1..127).collect(_.asAscii).select(_.isAlphaNum);
		numericalAlphabet = numericalAlphabet.collect(_.asString);
		numericalAlphabet = numericalAlphabet ++ ["pi", "+", "-", "."];

		accidentalAlphabet = (1..127).collect(_.asAscii).select(_.isDecDigit);
		accidentalAlphabet = accidentalAlphabet.collect(_.asString);
		accidentalAlphabet = accidentalAlphabet ++ ["s", "b"];
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

	runNumericalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"num_%_%_"++filenameSuffix;

		"".postln;
		"TestLexerBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runLexerTestsOnAlphabet(
			0, numericalAlphabetStringSizeLimit,
			numericalAlphabet, prefix, suffix, filenameFormat, "num"
		);
	}

	runAccidentalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"acc_%_%_"++filenameSuffix;

		"".postln;
		"TestLexerBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runLexerTestsOnAlphabet(
			0, accidentalAlphabetStringSizeLimit,
			accidentalAlphabet, prefix, suffix, filenameFormat, "acc"
		);
	}

}