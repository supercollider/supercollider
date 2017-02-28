// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : AbstractBrutalTest {
	classvar directory = "brutal_lexer_targeted_results/";

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {
		// init alphabets
		var alphanumAlphabet = (1..127).collect(_.asAscii).select(_.isAlphaNum);
		alphanumAlphabet = alphanumAlphabet.collect(_.asString);
		alphanumAlphabet = alphanumAlphabet ++ ["pi", "+", "-", "."];

		alphabets = [
			// alphanumeric characters plus [(pi)+-.]
			\alnum -> alphanumAlphabet,

			// for integer and floating-point parsing
			\num -> ["0", "1", "9", "pi", "+", "-", ".", "e"],

			// for accidental notation parsing (e.g., 1sss, 1s30)
			\acc -> ["0", "1", "9", "s", "b"]
		];

		alphabetStringLengths = [
			\alnum -> [1,2,3], // 0 and all prefixes covered by lexer tests
			\num -> [4,5], // 0..3 covered by alnum
			\acc -> [4,5,6] // 0..3 covered by alnum
		];
	}

	// TODO: refactor these as one method with a symbol switch
	runAlphanumLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			prefix, suffix, filenameFormat, \alnum, \compile
		);
	}

	runNumericalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			prefix, suffix, filenameFormat, \num, \compile
		);
	}

	runAccidentalLexerTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerTargetedBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		this.runTestsOnAlphabet(
			prefix, suffix, filenameFormat, \acc, \compile
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
