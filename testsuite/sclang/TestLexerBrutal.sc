// TestLexerBrutal.sc
// Brian Heim, 2017-02-26

TestLexerBrutal : UnitTest {
	// set to TRUE if you need `_expected` files
	classvar makingValidationFiles = false;

	// whether or not to include `^` in any alphabets. can't do it right now.
	classvar ignoreCaret = true;
	const caretAscii = 94;

	// alphabets used in testing
	classvar fullAlphabet; // -128..127
	classvar halfAlphabet; // 1..126

	*new {
		super.new.init;
	}

	init {
		// init alphabets
		fullAlphabet = (-128..-1) ++ (1..127);
		halfAlphabet = (1..126); // 127 is `DEL`

		if(ignoreCaret) {
			fullAlphabet = fullAlphabet.reject(_==caretAscii);
			halfAlphabet = halfAlphabet.reject(_==caretAscii);
		};

		fullAlphabet = fullAlphabet.collect(_.asAscii).collect(_.asString);
		halfAlphabet = halfAlphabet.collect(_.asAscii).collect(_.asString);
	}

}