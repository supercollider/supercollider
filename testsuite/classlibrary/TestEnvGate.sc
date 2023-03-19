
TestEnvGate : UnitTest {

	test_number_as_fadeTime {
		this.assertNoException({
			{ EnvGate(fadeTime:1) }.asSynthDef
		}, "number should be a valid EnvGate fadeTime")
	}

}

