// TestLexerTargetedBrutal.sc
// Brian Heim, 2017-02-27

TestLexerTargetedBrutal : AbstractLPCBrutalTest {

	outputDir { ^"lexer_targeted" }

	// set to `false` if you just want to generate output
	performingValidation { ^true }

	evaluationTechnique { ^\interpret }

	getAlphabets {
		var alphanumAlphabet = (1..127).collect(_.asAscii).select(_.isAlphaNum);
		alphanumAlphabet = alphanumAlphabet.collect(_.asString);
		alphanumAlphabet = alphanumAlphabet ++ ["pi", "+", "-", "."];

		^Dictionary[
			// alphanumeric characters plus [(pi)+-.]
			\alnum -> alphanumAlphabet,

			// for integer and floating-point parsing
			\num -> ["0", "1", "9", "pi", "+", "-", ".", "e"],

			// for accidental notation parsing (e.g., 1sss, 1s30)
			\acc -> ["0", "1", "9", "s", "b"],

			// for comments (line and block)
			\comment -> ["/", "*", "\n", "A", " "]
		];
	}

	getStringLengthsPerAlphabet {
		^stringLengthsPerAlphabet ? Dictionary[
			\alnum ->   [1,2,3],    // 0 and all prefixes covered by lexer tests
			\num ->     [4,5],      // 0..3 covered by alnum
			\acc ->     [4,5,6],    // 0..3 covered by alnum
			\comment -> [5,6,7,8,9] // 0..4 covered by lexer tests
		];
	}

	test_alnum_basic {
		this.runTestsOnAlphabet("",     "", "basic",      \alnum);
	}

	test_alnum_hexPrefixes {
		this.runTestsOnAlphabet("0x",   "", "hex",        \alnum);
		this.runTestsOnAlphabet("-0x",  "", "hexNeg",     \alnum);
	}

	test_alnum_radixPrefixes {
		this.runTestsOnAlphabet("1r",   "", "radix1",     \alnum);
		this.runTestsOnAlphabet("10r",  "", "radix10",    \alnum);
		this.runTestsOnAlphabet("12r",  "", "radix12",    \alnum);
		this.runTestsOnAlphabet("36r",  "", "radix36",    \alnum);
		this.runTestsOnAlphabet("37r",  "", "radix37",    \alnum);
		this.runTestsOnAlphabet("-10r", "", "radix10neg", \alnum);
		this.runTestsOnAlphabet("-12r", "", "radix12neg", \alnum);
	}

	test_num_basic {
		this.runTestsOnAlphabet("",     "", "basic",      \num);
		this.runTestsOnAlphabet("-",    "", "basicNeg",   \num);
	}

	test_acc_basic {
		this.runTestsOnAlphabet("",     "", "basic",      \acc);
		this.runTestsOnAlphabet("-",    "", "basicNeg",   \acc);
	}

	test_comment_basic {
		this.runTestsOnAlphabet("",     "", "basic",      \comment);
	}

}
