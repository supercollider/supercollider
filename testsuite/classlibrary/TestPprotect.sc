
TestPprotect : UnitTest {
	test_Pprotect_resetExceptionHandlerOnError {
		var success, cond = Condition.new,
		// Note that this must be a Stream, not a Pattern (x.asStream --> x).
		// If it's a pattern, then we don't have access to the routine
		// to check its exceptionHandler below.
		routine = Routine { 0.0.wait; 1 + nil },  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				{ cond.unhang }.defer;
			}
		).asStream;
		// Note that it is necessary to do this asynchronously!
		// Otherwise "routine"'s error will halt the entire test suite.
		stream.play;
		cond.hang;
		this.assert(success, "Pprotect should clear the stream's exceptionHandler");
	}
}
