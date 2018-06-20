
TestEnvGate : UnitTest {

	test_number_as_fadeTime {

		var error;
		try {
			{ EnvGate(fadeTime:1) }.asSynthDef
		} { |e| error = e };
		this.assert(error.isException.not, "number should be a valid EnvGate fadeTime")
	}

}

