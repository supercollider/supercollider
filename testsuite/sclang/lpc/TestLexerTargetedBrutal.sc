// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : AbstractLPCBrutalTest {

	outputDir { ^"lexer_targeted/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {
		// init alphabets
		var alphanumAlphabet = (1..127).collect(_.asAscii).select(_.isAlphaNum);
		alphanumAlphabet = alphanumAlphabet.collect(_.asString);
		alphanumAlphabet = alphanumAlphabet ++ ["pi", "+", "-", "."];

		alphabets = Dictionary[
			// alphanumeric characters plus [(pi)+-.]
			\alnum -> alphanumAlphabet,

			// for integer and floating-point parsing
			\num -> ["0", "1", "9", "pi", "+", "-", ".", "e"],

			// for accidental notation parsing (e.g., 1sss, 1s30)
			\acc -> ["0", "1", "9", "s", "b"]
		];

		alphabetStringLengths = Dictionary[
			\alnum -> [1,2,3], // 0 and all prefixes covered by lexer tests
			\num -> [4,5], // 0..3 covered by alnum
			\acc -> [4,5,6] // 0..3 covered by alnum
		];
	}

	runTargetedLexerTests {
		arg prefix, suffix, testMode, alphName;

		this.runTestsOnAlphabet(prefix, suffix, testMode, alphName, \compile);
	}

	test_alnum_basic {
		this.runTargetedLexerTests("", "", "basic", \alnum);
	}

	test_alnum_hexPrefixes {
		this.runTargetedLexerTests("0x", "", "hex", \alnum);
		this.runTargetedLexerTests("-0x", "", "hexNeg", \alnum);
	}

	test_alnum_radixPrefixes {
		this.runTargetedLexerTests("1r", "", "radix1", \alnum);
		this.runTargetedLexerTests("10r", "", "radix10", \alnum);
		this.runTargetedLexerTests("12r", "", "radix12", \alnum);
		this.runTargetedLexerTests("36r", "", "radix36", \alnum);
		this.runTargetedLexerTests("37r", "", "radix37", \alnum);
		this.runTargetedLexerTests("-10r", "", "radix10neg", \alnum);
		this.runTargetedLexerTests("-12r", "", "radix12neg", \alnum);
	}

	test_num_basic {
		this.runTargetedLexerTests("", "", "basic", \num);
		this.runTargetedLexerTests("-", "", "basicNeg", \num);
	}

	test_acc_basic {
		this.runTargetedLexerTests("", "", "basic", \acc);
		this.runTargetedLexerTests("-", "", "basicNeg", \acc);
	}

}
