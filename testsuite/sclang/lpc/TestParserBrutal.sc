// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : AbstractLPCBrutalTest {

	outputDir { ^"parser/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^false; }

	evaluationTechnique { ^\bytecode }

	getAlphabets {
		^Dictionary[
			\full -> [
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
				" size:",

				// literals
				" $a",
				" \"str\"",
				" 'sym'",
				"0",
				"1.1",
				" pi"
			],

			// a more targeted alphabet for block grammar
			\small -> [
				// keywords
				" var",
				" arg",
				" nil",

				// important punctuation
				" ;",
				" ,",
				" ...",
				" _",
				" |",

				// see above for explanations on omitting spaces here
				":",
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
			],

			// minimal alphabet for function/block grammar
			\mini -> [
				" ;",
				" ,",
				" ...",
				" |",

				" var",
				" arg",

				" abc",
				" def",
			]
		];
	}

	getStringLengthsPerAlphabet {
		^stringLengthsPerAlphabet ? Dictionary[
			// don't test on empty string - this is covered by compiler tests
			\full -> [1,2,3],
			\small -> [4,5],
			\mini -> [6,7]
		];
	}

	test_full_basic { this.runTestsTogglingTCO("", "", "basic", \full) }
	test_small_basic { this.runTestsTogglingTCO("", "", "basic", \small) }
	test_mini_basic { this.runTestsTogglingTCO("", "", "basic", \mini) }

	test_full_curlyBraceEnclose { this.runTestsTogglingTCO("{", "}", "curlyEnclose", \full) }
	test_small_curlyBraceEnclose { this.runTestsTogglingTCO("{", "}", "curlyEnclose", \small) }
	test_mini_curlyBraceEnclose { this.runTestsTogglingTCO("{", "}", "curlyEnclose", \mini) }
}
