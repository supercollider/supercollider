TestPdup : UnitTest {

	test_swappedArguments_doesNotCrash {
		var pat, stream, result;

		pat = Pbind(
			\dur, Pdup(1/8, 8)
		);

		stream = pat.asStream;

		// If this crashes, the test fails automatically
		result = stream.next(());

		this.assert(result.notNil);
	}

}
