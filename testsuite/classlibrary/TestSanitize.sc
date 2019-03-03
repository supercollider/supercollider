TestSanitize : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_sanitize {
		var duration, expected;
		var tests, testsIncomplete;
		duration = 256 / server.sampleRate;
		expected = 8888.0;

		// Not using a Dictionary here to retain order
		tests = [
			"Sanitize.kr(8888)" -> { Sanitize.kr(expected, 0) },
			"Sanitize.kr(inf)" -> { Sanitize.kr(inf, expected) },
			"Sanitize.kr(-inf)" -> { Sanitize.kr(inf, expected) },
			"Sanitize.kr(nan)" -> { Sanitize.kr(DC.kr(0) / 0, expected) },

			"Sanitize.ar(8888)" -> { Sanitize.ar(DC.ar(expected), 0) },
			"Sanitize.ar(inf)" -> { Sanitize.ar(DC.ar(inf), expected) },
			"Sanitize.ar(-inf)" -> { Sanitize.ar(DC.ar(-inf), expected) },
			"Sanitize.ar(nan)" -> { Sanitize.ar(DC.ar(0) / 0, expected) }
		];

		testsIncomplete = tests.size;

		tests.do { |test|
			var text, ugenGraphFunc;
			text = test.key;
			ugenGraphFunc = test.value;
			ugenGraphFunc.loadToFloatArray(duration, server, { |samples|
				this.assertArrayFloatEquals(samples, expected, text);
				testsIncomplete = testsIncomplete - 1;
			});
		};

		this.wait { testsIncomplete == 0 };
	}

}
