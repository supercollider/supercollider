// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : AbstractBrutalTest {
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

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

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

	// TODO: refactor these as one method with a symbol switch
	runAlphanumLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			0, alphanumAlphabetStringSizeLimit,
			alphanumAlphabet, prefix, suffix, filenameFormat, "alnum"
		);
	}

	runNumericalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			0, numericalAlphabetStringSizeLimit,
			numericalAlphabet, prefix, suffix, filenameFormat, "num"
		);
	}

	runAccidentalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			0, accidentalAlphabetStringSizeLimit,
			accidentalAlphabet, prefix, suffix, filenameFormat, "acc"
		);
	}

	test_alnum_basic {
		this.runAlphanumLexerTests("", "", "basic");
	}

	test_alnum_hexPrefixes {
		this.runAlphanumLexerTests("0x", "", "hex");
		this.runAlphanumLexerTests("-0x", "", "hexNeg");
	}

	test_alnum_radixPrefixes {
		this.runAlphanumLexerTests("1r", "", "radix1");
		this.runAlphanumLexerTests("10r", "", "radix10");
		this.runAlphanumLexerTests("12r", "", "radix12");
		this.runAlphanumLexerTests("36r", "", "radix36");
		this.runAlphanumLexerTests("37r", "", "radix37");
		this.runAlphanumLexerTests("-10r", "", "radix10neg");
		this.runAlphanumLexerTests("-12r", "", "radix12neg");
	}

	test_num_basic {
		this.runNumericalLexerTests("", "", "basic");
		this.runNumericalLexerTests("-", "", "basicNeg");
	}

	test_acc_basic {
		this.runAccidentalLexerTests("", "", "basic");
		this.runAccidentalLexerTests("-", "", "basicNeg");
	}

}
