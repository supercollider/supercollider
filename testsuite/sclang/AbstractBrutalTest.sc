// AbstractBrutalTest.sc
// Brian Heim, 2017-02-28

AbstractBrutalTest : UnitTest {
	// alphabets and associated data used in testing

	// Dictionary of alphabets (type Dictionary<Symbol, [String]>)
	var <>alphabets;

	// Dictionary of string lengths to test per alphabet
	// (type Dictionary<Symbol, [Integer]>)
	var <>alphabetStringLengths;

	*new {
		^super.new.initAlphabets;
	}

	// this method fills `alphabets` and `maxLengths`
	initAlphabets { ^this.subclassResponsibility(thisMethod) }

	// used to determine the output location
	outputDir { ^this.subclassResponsibility(thisMethod) }

	// should return `true` if you want to generate validation files,
	// and return `false` for ordinary testing
	makingValidationFiles { ^this.subclassResponsibility(thisMethod) }

	checkDiffs {
		arg diffs;

		if(diffs.isEmpty.not) {
			LexerParserCompilerTestUtils.printDiffs(diffs);
			this.failed(thisMethod, "Diffs were found between test and validation files");
		} {
			postf("%: no diffs found.\n".format(this.class.name));
		}
	}

	runTestsOnAlphabet {
		arg prefix, suffix, filenameFormat, alphabetName, technique;

		var diffs;

		alphabetStringLengths[alphabetName].do {
			arg len;

			diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				alphabets[alphabetName],
				len,
				prefix,
				suffix,
				filenameFormat.format(alphabetName, len),
				technique,
				this.makingValidationFiles,
				true
			);

			this.checkDiffs(diffs);
		};
	}

}
