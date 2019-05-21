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

	test_parseOnlyWhitepace {
		var source = "
    /* a single-line block comment */    \n"
"// single-line comment
\t// comment after tab
    /* multi
	 * line
	 * indented
	 * block
	 * comment */
\t
";
		var parser = Parser.newFromString(source);
		var expectedTypes = [\meta, \newLines, \spaces, \blockComment, \spaces, \newLines,
			\comment, \tabs, \comment, \spaces, \blockComment, \newLines, \tabs, \newLines];

		this.assertEquals(parser.getSource, source);
		this.assertEquals(parser.parseTree.size, expectedTypes.size);
		expectedTypes.do({ |type, i|
			this.assertEquals(parser.parseTree[i].at(\type), type);
			if (i > 0, {
				this.assertEquals(parser.parseTree[i].at(\containedBy), 0);
			}, {
				this.assertEquals(parser.parseTree[i].at(\containedBy), nil);
			});
		});
	}

	test_parseEmptyBlock {
		var source = "()";
		var parser = Parser.newFromString(source);
		var expectedTypes = [\meta, \block, \blockReturn];
		var expectedContainedBy = [nil, 0, 1];

		this.assertEquals(parser.getSource, source);
		this.assertEquals(parser.parseTree.size, expectedTypes.size);
		parser.parseTree.size.do({ |i|
			this.assertEquals(parser.parseTree[i].at(\type), expectedTypes[i]);
			this.assertEquals(parser.parseTree[i].at(\containedBy), expectedContainedBy[i]);
		});
	}

	test_parseBlockWithVariables {
		var source = "
// whitespace before blocks should not be included in blocks
(
var a;
)
// whitespace after blocks should not be included in blocks
";
		var parser = Parser.newFromString(source);
		var expectedTypes = [\meta, \newLines, \comment, \block, \newLines, \tabs, \variables,
			\variableList, \spaces, \variableDefinition, \end, \newLines, \blockReturn, \newLines,
			\comment, \newLines];
		var expectedContainedBy = [nil /* meta */, 0 /* newLines */, 0 /* comment */, 0 /* block */,
			3 /* newLines */, 3 /* tabs */, 3 /* variables */, 6 /* variableList */, 7 /* spaces */,
			7 /* variableDefinition */, 7 /* end */, 6 /* newLines */, 3 /* blockReturn */,
			0 /* newLines */, 0 /* comment */, 0 /* newLines */];

		this.assertEquals(parser.getSource, source);
		parser.parseTree.size.do({ |i|
			this.assertEquals(parser.parseTree[i].at(\type), expectedTypes[i]);
			this.assertEquals(parser.parseTree[i].at(\containedBy), expectedContainedBy[i]);
		});
	}

	test_parseBlockWithLiteralsAssignedToVariables {
		var source = "(
// Test different varieties of Integers.
var a = 0, b = -7, c = 0xfe, d = -0xA7DD, e = 2r1010, f = -8r70;
// Test different varieities of Floats.
//var g = 47.0, h = inf, i = -2.05pi, j = 8.07E-4, k = 12r4A.ABC, l = 2s204, m = 6bbbb;
// Test characters, including escaped ones.
//var aChar = $^; //
// Test strings including multiline.
//var aString = \"this is a string\";
//var aSymbol = \\bq;
//var aTrue = true;
//var aFalse = false;
//var aNil = nil;
//var aList = [ 'a', 1
)";
		var parser = Parser.newFromString(source);
		this.assertEquals(parser.getSource, source);
	}

	// how about a = b = c and such?
}
