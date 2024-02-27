TestPipe : UnitTest {

	test_basic_pipe {
        if (thisProcess.platform.isKindOf(UnixPlatform)) {
            var pipe = Pipe.argv(["date", "+%H"], "r");
            var result = pipe.getLine();
            assert("^[0-9]+$".matchRegexp(result)))
        }
	}

}
