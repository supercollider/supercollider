// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : AbstractBrutalTest {
	// alphabets used in testing
	classvar fullAlphabet;
	classvar smallAlphabet;
	classvar miniAlphabet;

	classvar fullAlphabetStringSizeLimit = 4;
	classvar smallAlphabetStringSizeLimit = 6;
	classvar miniAlphabetStringSizeLimit = 8;

	classvar directory = "brutal_parser_results/";

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

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
			// " while", // technically a keyword, but it doesn't really get
			             // treated any differently
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
			// " <>", // only legal in class files
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
			" abc:",

			// literals
			" $a",
			" \"str\"",
			// " \\sym", // this is a parser test, and in the context of the parser
			             // the different symbol notations are equivalent
			" 'sym'",
			"0",
			"1.1",
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

			" var",
			" arg",

			" abc",
			" def",
		];
	}

	runParserTests {
		arg prefix, suffix, filenameSuffix;

		var filenameFormat = directory++"%_%_"++filenameSuffix;

		"".postln;
		"TestParserBrutal: running test mode %".format(filenameSuffix).underlined.postln;

		// test on full alphabet
		this.runTestsOnAlphabet(
			0, fullAlphabetStringSizeLimit,
			fullAlphabet, prefix, suffix, filenameFormat, "full", \bytecode
		);

		// test on small alphabet
		this.runTestsOnAlphabet(
			fullAlphabetStringSizeLimit+1, smallAlphabetStringSizeLimit,
			smallAlphabet, prefix, suffix, filenameFormat, "small", \bytecode
		);

		// test on mini alphabet
		this.runTestsOnAlphabet(
			smallAlphabetStringSizeLimit+1, miniAlphabetStringSizeLimit,
			miniAlphabet, prefix, suffix, filenameFormat, "mini", \bytecode
		);
	}

	test_basic {
		this.runParserTests("", "", thisMethod.name.asString[5..]);
	}

	test_curlyBraceEnclose {
		this.runParserTests("{", "}", thisMethod.name.asString[5..]);
	}
}
