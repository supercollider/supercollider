
TestPprotect : UnitTest {
	test_PprotectRecursion {
		var success, cond = Condition.new,
		routine = Routine { 0.0.wait; 1 + nil },  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				{ cond.unhang }.defer;
			}
		).asStream;
		// note that it is necessary to do this asynchronously!
		// otherwise "routine"'s error will halt the entire test suite
		stream.play;
		cond.hang;
		this.assert(success, "Pprotect should clear the stream's exceptionHandler");
	}
}
