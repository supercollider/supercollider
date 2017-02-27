// TestLexerBrutal.sc
// Brian Heim, 2017-02-26

TestLexerBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = false;

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
		} {
			postln("TestLexerBrutal: no diffs found.");
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

	runLexerTests {
		arg prefix, suffix, filenameSuffix;


		var filenameFormat = directory++"lexer_%_%_"++filenameSuffix;

		"".postln;
		"TestLexerBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runParserTestsOnAlphabet(
			0, fullAlphabetStringSizeLimit,
			fullAlphabet, prefix, suffix, filenameFormat, "full"
		);

		// test on small alphabet
		this.runParserTestsOnAlphabet(
			fullAlphabetStringSizeLimit+1, halfAlphabetStringSizeLimit,
			halfAlphabet, prefix, suffix, filenameFormat, "half"
		);
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
