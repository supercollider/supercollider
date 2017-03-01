// TestCompilerBrutal.sc
// Brian Heim, 2017-02-28

TestCompilerBrutal : AbstractBrutalTest {

	outputDir { ^"valid_compiler/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {
		// TODO
		// Alphabets ought to make use of every possible bytecode the interpreter
		// is capable of emitting while parsing cmd line.

		// alphabets = Dictionary[];

		// alphabetStringLenghts = Dictionary[];
	}

	runCompilerTests {
		// TODO
	}

	test_basic {
		// TODO
	}

}