// TestLexerBrutal.sc
// Brian Heim, 2017-02-26

TestLexerBrutal : AbstractBrutalTest {
	// whether or not to include `^` in any alphabets. Can't do it right now,
	// because ^ will cause unwanted early termination when interpreted.
	classvar ignoringCaret = true;
	const caretAscii = 94;

	outputDir { ^"valid_lexer/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {

		alphabets = Dictionary[
			// all 8-bit values except 0 `NUL`
			\full -> ((-128..-1) ++ (1..127)),

			// all ASCII values except 0 `NUL` and 7F `DEL`
			\half -> (1..126),

			// all punctuation, plus [09AZaz]
			\mini -> ((32..48) ++ (57..65) ++ (90..97) ++ (122..126))
		];

		// convert each alphabet to correct string representation,
		// and also remove `^` if necessary.
		alphabets.keysValuesChange {
			arg name, alphabet;

			if(ignoringCaret) {
				alphabet = alphabet.reject(_==caretAscii);
			};

			alphabet.collect(_.asAscii).collect(_.asString);
		};

		alphabetStringLengths = Dictionary[
			\full -> [0,1,2],
			\half -> [3],
			\mini -> [4]
		];
	}

	runLexerTests {
		arg prefix, suffix, testMode;

		alphabets.keysDo {
			arg key;
			this.runTestsOnAlphabet(prefix, suffix, testMode, key, \compile);
		}
	}

	test_basic {
		this.runLexerTests("", "", "basic");
	}

	test_lineCommentPrefix {
		this.runLexerTests("//", "", "lineCommentPrefix");
	}

	test_blockCommentPrefix {
		this.runLexerTests("/*", "", "blockCommentPrefix");
	}

	test_blockCommentSuffix {
		this.runLexerTests("", "*/", "blockCommentSuffix");
	}

	test_blockCommentEnclose {
		this.runLexerTests("/*", "*/", "blockCommentEnclose");
	}

	test_semanticPrefix {
		this.runLexerTests("\\unique;", "", "semanticPrefix");
	}

	test_semanticSuffix {
		this.runLexerTests("", ";\\unique", "semanticSuffix");
	}
}
