// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : AbstractLPCBrutalTest {

	outputDir { ^"parser" }

	// set to `false` if you just want to generate output
	performingValidation { ^true }

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
				// " while", // technically a keyword, but it doesn't get
				             // treated any differently in practice
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
			\full ->  [1,2,3,4],
			\small -> [5],
			\mini ->  [6,7]
		];
	}

	// full alphabet tests
	test_full_basic           { this.runTestsTogglingTCO("",      "",  "basic",           \full) }
	test_full_block           { this.runTestsTogglingTCO("{",     "}", "block",           \full) }

	// small alphabet tests
	test_small_basic          { this.runTestsTogglingTCO("",      "",  "basic",           \small) }
	test_small_block          { this.runTestsTogglingTCO("{",     "}", "block",           \small) }

	// mini alphabet tests
	test_mini_varPrefix       { this.runTestsTogglingTCO("var ",  "",  "varPrefix",       \mini) }
	test_mini_blockVarPrefix  { this.runTestsTogglingTCO("{var ", "}", "blockVarPrefix",  \mini) }
	test_mini_blockArgPrefix  { this.runTestsTogglingTCO("{arg ", "}", "blockArgPrefix",  \mini) }
	test_mini_blockPipePrefix { this.runTestsTogglingTCO("{|",    "}", "blockPipePrefix", \mini) }
}
