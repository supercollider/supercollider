TestNodeProxy : UnitTest {

	var server, proxy;

	setUp {
		// NOTE: these tests don't make use of a booted server
		server = Server(this.class.name);
		proxy = NodeProxy(server);
	}

	tearDown {
		proxy.clear;
		server.remove;
	}

	test_source_synthDef_build {
		var x = nil;

		proxy.source = { x = true; Silent.ar };
		this.assertEquals(x, true, "Setting a source should build the SynthDef");
	}

	test_source_rate {
		proxy.source = { SinOsc.ar };
		this.assertEquals(proxy.rate, \audio, "An audio rate ugen graph should init proxy to \audio rate");
	}

	test_source_numChannels {
		proxy.source = { Silent.ar.dup(3) };
		this.assertEquals(proxy.numChannels, 3, "A 3 channel ugen graph should init proxy to 3 channels");
	}

	test_source_proxy_loaded {
		proxy.source = { Silent.ar };
		this.assertEquals(proxy.loaded, false, "Setting the proxy's source should not set loaded = true");
	}

	test_source_proxy_isPlaying {
		proxy.source = { Silent.ar };
		this.assertEquals(proxy.isPlaying, false, "Setting the proxy's source should not set isPlaying = true");
	}

	test_asCode_basic {
		var codeString = "a = NodeProxy.new(Server.fromName( 'TestNodeProxy' ));\n";

		this.assertEquals(proxy.asCode.postcs, codeString,
			"asCode-posting basic nodeproxy should post valid source code.",
			false);
	}

	test_asCode_single {
		var codeString = "a = NodeProxy.new(Server.fromName( 'TestNodeProxy' )).source_({ DC.ar });\n";
		proxy.source = { DC.ar };

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting single-source nodeproxy should post simple source form.",
			false
		);
	}

	test_asCode_multi {
		var asCodeString =
		"(
a = NodeProxy.new;
a[5] = { DC.ar };
a[10] = { DC.ar(0.01) };

);
";
		proxy[5] = { DC.ar };
		proxy[10] = { DC.ar(0.01) };
		this.assertEquals(proxy.asCode.postcs, asCodeString,
			"asCode-posting multi-source nodeproxy asCode should post all its sources.",
			false
		);
	}

	test_asCode_settings {
		var codeString =
		"(\n"
		"a = NodeProxy.new(Server.fromName( 'TestNodeProxy' ));\n"
		"a.set('freq', 440);\n"
		");\n";
		proxy.set('freq', 440);

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting nodeproxy with settings should post these correctly."
		);
	}

	// this one needs the server booted ...
	test_asCode_playState {
		var codeString =
		"(\n"
		"a = NodeProxy.new(Server.fromName( 'TestNodeProxy' ));\n"
		"a.play(\n"
		"	out: 8, \n"
		"	vol: 0.25\n"
		"\n"
		");\n"
		");\n";

		this.bootServer(server);
		proxy.play(8, 2, vol: 0.25);

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting nodeproxy with settings should post these correctly."
		);
	}

	test_asCode_single_ndef {
		var codeString = "Ndef('x', { DC.ar });\n";
		proxy = Ndef(\x, { DC.ar });

		proxy.asCode.cs;
		codeString.cs;
		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting single-source Ndef should post correctly."
		);

		Ndef(\x).free.clear;
	}

	test_asCode_multi_ndef {
		var codeString =
		"(\n"
		"Ndef('x')[5] = { DC.ar };\n"
		"Ndef('x')[10] = { DC.ar(0.01) };\n"
		"\n"
		");\n";

		proxy = Ndef(\x);
		Ndef(\x)[5] = { DC.ar };
		Ndef(\x)[10] = { DC.ar(0.01) };

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting single-source Ndef should post correctly."
		);

		Ndef(\x).free.clear;
	}
}
