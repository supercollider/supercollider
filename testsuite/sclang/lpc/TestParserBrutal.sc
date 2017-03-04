// TestParserBrutal.sc
// Brian Heim, 2017-2-26

TestParserBrutal : AbstractLPCBrutalTest {

	outputDir { ^"parser/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {
		// init alphabets

		alphabets = Dictionary[
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

		alphabetStringLengths = Dictionary[
			// don't test on empty string - this is covered by compiler tests
			\full -> [1,2,3,4],
			\small -> [5,6],
			\mini -> [7,8]
		]
	}

	runParserTests {
		arg prefix, suffix, testMode, tcoMode;

		var tco = Process.tailCallOptimize;
		Process.tailCallOptimize_(tcoMode);

		alphabets.keysDo {
			arg key;
			this.runTestsOnAlphabet(prefix, suffix, testMode, key, \bytecode);
		};

		Process.tailCallOptimize_(tco);
	}

	test_basicTCO {
		this.runParserTests("", "", "basicTCO", true);
	}

	test_curlyBraceEncloseTCO {
		this.runParserTests("{", "}", "curlyEncloseTCO", true);
	}

	test_basicNoTCO {
		this.runParserTests("", "", "basicNoTCO", false);
	}

	test_curlyBraceEncloseNoTCO {
		this.runParserTests("{", "}", "curlyEncloseNoTCO", false);
	}
}
