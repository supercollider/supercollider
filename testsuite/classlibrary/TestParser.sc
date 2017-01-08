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
}
