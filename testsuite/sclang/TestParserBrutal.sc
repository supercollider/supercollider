// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = false;

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
}