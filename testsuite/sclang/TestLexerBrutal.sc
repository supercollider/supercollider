// TestLexerBrutal.sc
// Brian Heim, 2017-02-26

TestLexerBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = true;

	// whether or not to include `^` in any alphabets. can't do it right now.
	classvar ignoreCaret = true;
	const caretAscii = 94;

	// alphabets used in testing
	classvar fullAlphabet; // -128..127
	classvar halfAlphabet; // 1..126

	// maximum string size for testing full and half alphabets
	// the half alphabet is only tested on strings of length
	// (fullLimit+1) to (halfLimit)
	classvar fullAlphabetStringSizeLimit = 2;
	classvar halfAlphabetStringSizeLimit = 3;

	classvar directory = "brutal_lexer_results/";

	*new {
		^super.new.initAlphabets;
	}

	initAlphabets {
		// init alphabets
		fullAlphabet = (-128..-1) ++ (1..127);
		halfAlphabet = (1..126); // 127 is `DEL`

		if(ignoreCaret) {
			fullAlphabet = fullAlphabet.reject(_==caretAscii);
			halfAlphabet = halfAlphabet.reject(_==caretAscii);
		};

		fullAlphabet = fullAlphabet.collect(_.asAscii).collect(_.asString);
		halfAlphabet = halfAlphabet.collect(_.asAscii).collect(_.asString);
	}

	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		}
	}

	runLexerTests {
		arg prefix, suffix, filenameSuffix;

		var diffs;

		var filenameFormat = directory++"lexer_%_%_"++filenameSuffix;

		"TestLexerBrutal: running test mode %\n".postf(filenameSuffix);

		(0..fullAlphabetStringSizeLimit).do {
			arg len;

			diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				fullAlphabet,
				len,
				prefix,
				suffix,
				filenameFormat.format("full", len),
				\compile,
				makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};

		(fullAlphabetStringSizeLimit+1..halfAlphabetStringSizeLimit).do {
			arg len;

			diffs = diffs ++ LexerParserCompilerTestUtils.testAllPossibleStrings(
				halfAlphabet,
				len,
				prefix,
				suffix,
				filenameFormat.format("half", len),
				\compile,
				makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};
	}

	test_basic {
		this.runLexerTests("", "", thisMethod.name.asString[5..]);
	}

	test_lineCommentPrefix {
		this.runLexerTests("//", "", thisMethod.name.asString[5..]);
	}

	test_blockCommentPrefix {
		this.runLexerTests("/*", "", thisMethod.name.asString[5..]);
	}

	test_blockCommentSuffix {
		this.runLexerTests("", "*/", thisMethod.name.asString[5..]);
	}

	test_blockCommentEnclose {
		this.runLexerTests("/*", "*/", thisMethod.name.asString[5..]);
	}

	test_semanticPrefix {
		this.runLexerTests("\\unique;", "", thisMethod.name.asString[5..]);
	}

	test_semanticSuffix {
		this.runLexerTests("", "\\unique;", thisMethod.name.asString[5..]);
	}
}
