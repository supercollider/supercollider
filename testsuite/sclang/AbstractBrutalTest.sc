// AbstractBrutalTest.sc
// Brian Heim, 2017-02-28

AbstractBrutalTest : UnitTest {
	*new {
		^super.new.initAlphabets;
	}

	initAlphabets { ^this.subclassResponsibility(thisMethod) }

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
		arg min, max, alphabet, prefix, suffix, filenameFormat, alphabetName, technique;

		var diffs;

		(min..max).do {
			arg len;

			diffs = LexerParserCompilerTestUtils.testAllPossibleStrings(
				alphabet,
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
