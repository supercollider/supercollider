TestPdup : UnitTest {

	test_swappedArguments_doesNotCrash {
		var pat, stream, result;

		pat = Pbind(
			\dur, Pdup(1/8, 8)
		);

		stream = pat.asStream;

		// Use a proper event to avoid hanging the interpreter
		result = stream.next(Event.default);

		this.assert(result.notNil);
	}

}
