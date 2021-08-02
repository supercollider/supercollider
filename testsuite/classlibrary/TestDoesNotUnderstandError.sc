TestDoesNotUnderstandError : UnitTest {
	test_construction {
		var error;
		try { 3.bogusTestMethodNameXyz(4, 5) } { |e| error = e };

		this.assertEquals(error.receiver, 3);
		this.assertEquals(error.selector, 'bogusTestMethodNameXyz');
		this.assertEquals(error.args, [4, 5]);
		this.assertEquals(error.suggestedCorrection, nil);
	}

	test_suggestedCorrection {
		var error;
		try { Object().szie } { |e| error = e };

		this.assertEquals(error.suggestedCorrection, Object.findMethod(\size));
	}
}
