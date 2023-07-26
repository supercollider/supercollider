// TestLexerBrutal.sc
// Brian Heim, 2017-02-26

TestLexerBrutal : AbstractLPCBrutalTest {
	// whether or not to include `^` in any alphabets. Can't do it right now,
	// because ^ will cause unwanted early termination when interpreted.
	classvar ignoringCaret = true;
	const caretAscii = 94;

	outputDir { ^"lexer" }

	// set to `false` if you just want to generate output
	performingValidation { ^true }

	evaluationTechnique { ^\interpret }

	getAlphabets {

		var tmp = Dictionary[
			// all 8-bit values except 0 `NUL`
			\full -> ((-128..-1) ++ (1..127)),

			// all ASCII values except 0 `NUL` and 7F `DEL`
			\half -> (1..126),

			// all punctuation, plus [09AZaz]
			\mini -> ((32..48) ++ (57..65) ++ (90..97) ++ (122..126))
		];

		// convert each alphabet to correct string representation,
		// and also remove `^` if necessary.
		^tmp.keysValuesChange {
			arg name, alphabet;

			if(ignoringCaret) {
				alphabet = alphabet.reject(_==caretAscii);
			};

			alphabet.collect(_.asAscii).collect(_.asString);
		};
	}

	getStringLengthsPerAlphabet {
		^stringLengthsPerAlphabet ? Dictionary[
			\full -> [0,1,2],
			\half -> [3],
			\mini -> [4]
		];
	}

	// full tests: just basic. the others are covered for all practical purposes by half/mini
	test_full_basic              { this.runTestsOnAlphabet("",          "",          "basic",              \full) }

	// half alphabet tests
	test_half_basic              { this.runTestsOnAlphabet("",          "",          "basic",              \half) }
	test_half_blockCommentPrefix { this.runTestsOnAlphabet("/*",        "",          "blockCommentPrefix", \half) }
	test_half_semanticPrefix     { this.runTestsOnAlphabet("\\unique;", "",          "semanticPrefix",     \half) }
	test_half_semanticSuffix     { this.runTestsOnAlphabet("",          ";\\unique", "semanticSuffix",     \half) }

	// mini alphabet tests
	test_mini_basic              { this.runTestsOnAlphabet("",          "",          "basic",              \mini) }

}
