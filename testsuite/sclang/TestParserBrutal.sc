// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = true;

	// alphabets used in testing
	classvar fullAlphabet;
	classvar smallAlphabet;
	classvar miniAlphabet;

	classvar fullAlphabetStringSizeLimit = 4;
	classvar smallAlphabetStringSizeLimit = 6;
	classvar miniAlphabetStringSizeLimit = 8;

	classvar directory = "brutal_parser_results/";

	*new {
		^super.new.initAlphabets;
	}

	initAlphabets {
		// init alphabets

		fullAlphabet = [
			// brackets
			"(",
			")",
			"{",
			"}",
			"[",
			"]",

			// keywords
			" var",
			" arg",
			" while",
			" true",
			" false",
			" nil",
			// " inf", // not actually interpreted by the parser

			// important punctuation
			" ;",
			" ,",
			" .",
			" `",
			" #",
			" !",
			" ..",
			" ...",
			" _",
			" <>",
			" <",
			" >",
			" -",
			" *",
			" +",
			" |",
			" <-",
			" ^",

			// these ones not preceded by a space
			// for grammatical reasons

			// `2 pow : 2` isn't legal
			":",
			// allows for `==`, `!=`
			"=",

			// identifiers
			" Object",
			" abc",
			" def",
			" ~abc",
			" ~def",
			" var abc",
			" var def",

			// method call
			".size", // size can be called on anything (defined on Object)

			// literals
			" $a",
			" \"str\"",
			" \\sym",
			" 'sym2'",
			"0",
			"1",
			" pi"
		];

		// a more targeted alphabet for block grammar
		smallAlphabet = [
			// keywords
			" var",
			" arg",
			" nil",

			// important punctuation
			" ;",
			" ,",
			// " .",
			// " `",
			// " #",
			// " !",
			// " ..",
			" ...",
			" _",
			// " <>",
			// " <",
			// " >",
			// " -",
			// " *",
			// " +",
			" |",
			// " <-",
			// " ^",

			// these ones not preceded by a space
			// for grammatical reasons

			// `2 pow : 2` isn't legal
			":",
			// allows for `==`, `!=`
			"=",

			// identifiers
			" Object",
			" abc",
			" def",
			" var abc",
			" var def",

			// literals
			"0",
			"1",
		];

		// minimal alphabet for function/block grammar
		miniAlphabet = [
			" ;",
			" ,",
			" ...",
			" |",

			"=",

			" var",
			" arg",

			" abc",
			" def",
		];
	}

	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		} {
			postln("TestParserBrutal: no diffs found.");
		}
	}

	runParserTestsOnAlphabet {
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
				\bytecode,
				makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};
	}

	runParserTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"parser_%_%_"++filenameSuffix;

		"".postln;
		"TestParserBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runParserTestsOnAlphabet(
			0, fullAlphabetStringSizeLimit,
			fullAlphabet, prefix, suffix, filenameFormat, "full"
		);

		// test on small alphabet
		this.runParserTestsOnAlphabet(
			fullAlphabetStringSizeLimit+1, smallAlphabetStringSizeLimit,
			smallAlphabet, prefix, suffix, filenameFormat, "small"
		);

		// test on mini alphabet
		this.runParserTestsOnAlphabet(
			smallAlphabetStringSizeLimit+1, miniAlphabetStringSizeLimit,
			miniAlphabet, prefix, suffix, filenameFormat, "mini"
		);
	}

	test_basic {
		this.runParserTests("", "", thisMethod.name.asString[5..]);
	}

	test_curlyBraceEnclose {
		this.runParserTests("{", "}", thisMethod.name.asString[5..]);
	}
}
