// Formatting is on purpose here to catch edge cases.
// Don't change it.
TestSourceCode : UnitTest {
	test_fn {
		var f;

		f = { 1 + 1 };
		this.assertEquals(f.def.sourceCode.quote, "{ 1 + 1 }".quote);

		f = {1+1};
		this.assertEquals(f.def.sourceCode.quote, "{1+1}".quote);

		f = {1+1} 
		/* dont remove this comment */
		;
		this.assertEquals(f.def.sourceCode.quote, "{1+1}".quote);

		f = { var a, b = 10;



		};
		this.assertEquals(f.def.sourceCode.quote, "{ var a, b = 10;



		}".quote);
	}

	test_partial {
		// Right now, only 'closed' functions store their source code.
		// Once this is fixed, uncomment below.
// 		var f;

// 		f = _ + 1;
// 		this.assertEquals(f.def.sourceCode.quote, "_ + 1".quote);

// 		f = (_ + 1)
// 		// dont' remove this comment or new line
// 		;
// 		this.assertEquals(f.def.sourceCode.quote, "(_ + 1)".quote);

// 		f = (_ + 1);
// 		this.assertEquals(f.def.sourceCode.quote, "(_ + 1)".quote);

// 		f = _



// 		+
// // a

// 		1



// 		;
// 		this.assertEquals(f.def.sourceCode.quote, "_



// 		+
// // a

// 		1".quote);


// 		f = _ + _ + _ + _;
// 		this.assertEquals(f.def.sourceCode.quote, "_ + _ + _ + _".quote);

// 		f = (_ + (_ + (_)) + _);
// 		this.assertEquals(f.def.sourceCode.quote, "(_ + (_ + (_)) + _)".quote);
	}
}
