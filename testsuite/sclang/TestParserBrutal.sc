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

	}
}