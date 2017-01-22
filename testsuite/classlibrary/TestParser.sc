TestParser : UnitTest {

	// Regression tests for a specific bug in block comment parsing.
	// See https://github.com/supercollider/supercollider/issues/2624
	test_blockComments {
		var openTests, closedTests, errorTests;
		var testCs, testCsValue;

		/* A test compile string, and its interpreted value.
		* Will be added to the end of a test comment to make sure it
		* does/doesn't get interpreted as expected. Will also be
		* added to the start of an error-causing string to make sure
		* it causes an error.
		*/
		testCs = " 3\n";
		testCsValue = 3;

		// Tests open-ended block comments; these should return 'nil' if you add any text after them.
		openTests = [
			"/*", // crazy strings first
			"/*/",
			"/*/*",
			"/*/*/",
			"/*/*/*",
			"/*/* */",
			"/*/**/*",
			"/*/* */*",
			"/*/*/*/",
			"/*/*/*/*",
			"/*//* */", // '//*' interpreted as '/*' here

			"/* /*", // sanity check
			"/* /* */",
			"/* /* /* */ */"
		];

		// Tests closed block comments; code after one of these strings will be executed.
		closedTests = [
			"/**/",
			"/*/**/*/",
			"/*/*/**/*/*/",
			"/*// */",
			"/* // */", // line comment begun in block comment doesn't count
			"/*//* */ */", // '//*' interpreted as '/*' here

			"/* */", // sanity check
			"/* /* */ */",
			"/* /* /* */ */ */"
		];

		/* These should throw an error when interpreted.
		* Since the errors from .interpret cannot be caught, this just tests
		* that it returns nil no matter where the test string is added.
		*/
		errorTests = [
			"*/",
			"*/*",
			"*/*/",
			"/**/*",
			"/**/*/",
			"/**/ */",
			"/* */*/",
			"/* */ */",
			"/*/**/*/*",
			"/*/**/*/*/",

			"///*\n*/", // block comment begun in line comment doesn't count (this would probably make the lang blow up if it failed anyway)
			"// /*\n*/"
		];

		openTests.do {
			|commentString|
			var message = "'%' should be an open-ended comment.".format(commentString);

			this.assertEquals(commentString.interpret, nil, message);
			this.assertEquals((commentString ++ testCs).interpret, nil, message);
		};

		closedTests.do {
			|commentString|
			var message = "'%' should be a closed block comment.".format(commentString);

			this.assertEquals(commentString.interpret, nil, message);
			this.assertEquals((commentString ++ testCs).interpret, testCsValue, message);
		};

		errorTests.do {
			|commentString|
			var message = "'%' should throw a parsing error when interpreted.".format(commentString);
			var courtesyMessage = "NOTE: The parsing errors thrown above are intentional — currently testing bizarre strings with .interpret";

			this.assertEquals(commentString.interpret, nil, message);
			this.assertEquals((commentString ++ testCs).interpret, nil, message);
			this.assertEquals((testCs ++ commentString).interpret, nil, message);

			courtesyMessage.postln;
		}
	}

	test_symbolLiterals {
		// if something is broken, these will fail just by virtue of being declared.
		var testString = "12ab_";
		var symbolStrings = Array.fill(string.size, string.rotate(_));
		var messageProto = "'%' should be a legal symbol literal when";

		var testCs = ";3";
		var testCsValue = 3;

		symbolStrings.do {
			|symbolString|
			var message = messageProto.format(symbolString) + "preceded by backslash";
			var interpretString = "\\" ++ symbolString ++ testCs;

			this.assertEquals(interpretString.interpret, testCsValue, message);

			message = messageProto.format(symbolString) + "enclosed in single quotes";
			interpretString = "'" ++ symbolString ++ "'" ++ testCs;

			this.assertEquals(interpretString.interpret, testCsValue, message);
		}

		// symbols containing spaces are valid with single quotes, but not with backslash
		testString = "12 ab _";
		symbolStrings = Array.fill(string.size, string.rotate(_));
		messageProto = "'%' should be % symbol when";

		symbolStrings.do {
			|symbolString|
			var courtesyMessage = "NOTE: The parsing error thrown above is intentional — currently testing bizarre symbols with .interpret";
			var message = messageProto.format(symbolString, "an illegal") + "preceded by backslash";
			var interpretString = "\\" ++ symbolString ++ testCs;

			this.assertEquals(interpretString.interpret, nil, message);
			courtesyMessage.postln;

			message = messageProto.format(symbolString, "a legal") + "enclosed in single quotes";
			interpretString = "'" ++ symbolString ++ "'" ++ testCs;

			this.assertEquals(interpretString.interpret, testCsValue, message);
		}

		// escape sequences just give the escaped character with single quotes, but are illegal with backslash
		// test every char from space (32) to ~ (126) -- 127 is non-printing
		symbolStrings = (32..126).collect({
			|x|
			"\\" ++ x.asAscii;
		});

		messageProto = "'%' should be % symbol when";

		symbolStrings.do {
			|symbolString|
			var courtesyMessage = "NOTE: The parsing error thrown above is intentional — currently testing bizarre symbols with .interpret";
			var message;
			var interpretString;

			// test backslash as error-producing
			interpretString = "\\" ++ symbolString ++ testCs;
			message = "'%' should be an illegal symbol when preceded by backslash".format(symbolString);
			this.assertEquals(interpretString.interpret, nil, message);
			courtesyMessage.postln;

			// test single quotes as successful
			interpretString = "\'" ++ symbolString ++ "\'";
			message = "'%' should be the same as '%' when enclosed in single quotes".format(symbolString, symbolString[1]);
			this.assertEquals(interpretString.interpret.asString, symbolString[1].asSymbol, message);
		}
	}
}
