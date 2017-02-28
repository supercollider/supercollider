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
	classvar smallAlphabet; // 1..126
	classvar miniAlphabet; // all punctuation, [019AZaz]

	// maximum string size for testing full and small alphabets
	// the small alphabet is only tested on strings of length
	// (fullLimit+1) to (smallLimit)
	classvar fullAlphabetStringSizeLimit = 2;
	classvar smallAlphabetStringSizeLimit = 3;
	classvar miniAlphabetStringSizeLimit = 4;

	classvar directory = "brutal_lexer_results/";

	*new {
		^super.new.initAlphabets;
	}

	initAlphabets {
		// init alphabets
		var alphabets = [
			(-128..-1) ++ (1..127),
			(1..126), // 127 is `DEL`
			// all punctuation, plus [019AZaz]
			(32..49) ++ (57..65) ++ (90..97) ++ (122..126)
		];

		if(ignoreCaret) {
			alphabets = alphabets.collect {
				arg alphabet;
				alphabet = alphabet.reject(_==caretAscii);
			}
		};

		alphabets = alphabets.collect {
			arg alphabet;
			alphabet = alphabet.collect(_.asAscii).collect(_.asString);
		};

		fullAlphabet = alphabets[0];
		smallAlphabet = alphabets[1];
		miniAlphabet = alphabets[2];
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

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestLexerBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runLexerTestsOnAlphabet(
			0, fullAlphabetStringSizeLimit,
			fullAlphabet, prefix, suffix, filenameFormat, "full"
		);

		// test on small alphabet
		this.runLexerTestsOnAlphabet(
			fullAlphabetStringSizeLimit+1, smallAlphabetStringSizeLimit,
			smallAlphabet, prefix, suffix, filenameFormat, "small"
		);

		// test on mini alphabet
		this.runLexerTestsOnAlphabet(
			smallAlphabetStringSizeLimit+1, miniAlphabetStringSizeLimit,
			miniAlphabet, prefix, suffix, filenameFormat, "mini"
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
		this.runLexerTests("", ";\\unique", thisMethod.name.asString[5..]);
	}
}
