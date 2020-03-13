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

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting basic nodeproxy should post valid source code.");
	}

	test_asCode_single {
		var codeString = "a = NodeProxy.new(Server.fromName( 'TestNodeProxy' )).source_({ DC.ar });\n";
		proxy.source = { DC.ar };

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting single-source nodeproxy should post simple source form.");
	}

	test_asCode_multi {
		var asCodeString =
		"(\n"
		"a = NodeProxy.new;\n"
		"a[5] = { DC.ar };\n"
		"a[10] = { DC.ar(0.01) };\n"
		"\n"
		");\n";
		proxy[5] = { DC.ar };
		proxy[10] = { DC.ar(0.01) };
		this.assertEquals(proxy.asCode, asCodeString,
			"asCode-posting multi-source nodeproxy asCode should post all its sources.");
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

		server.sync; // avoids "FAILURE IN SERVER /g_new Group 1 not found"

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting nodeproxy with settings should post these correctly."
		);

		server.quit;
	}

	test_asCode_single_ndef {
		var codeString = "Ndef('x', { DC.ar });\n";
		proxy = Ndef(\x, { DC.ar });

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

	test_asCode_single_inProxySpace {
		var codeString = "~a = { DC.ar };\n";
		var p = ProxySpace.push;
		p[\a] = { DC.ar };
		p[\a].asCode.cs;

		this.assertEquals(p[\a].asCode(envir: p), codeString,
			"asCode-posting single-source proxy should post by key in pushed proxyspace."
		);

		p[\a].free;
		p.pop;
	}

	test_asCode_multi_inProxySpace {
		var codeString =
		"(\n"
		"~a[5] = { DC.ar };\n"
		"~a[10] = { DC.ar(0.01) };\n"
		"\n"
		");\n";

		var p = ProxySpace.push;
		p[\a][5] = { DC.ar };
		p[\a][10] = { DC.ar(0.01) };

		this.assertEquals(p[\a].asCode(envir: p), codeString,
			"asCode-posting multi-source proxy should post by key in pushed proxyspace."
		);

		p[\a].free;
		p.pop;
	}

	test_schedAfterFade_notPlaying {
		var ok = false;
		proxy.fadeTime = 0.1;
		proxy.schedAfterFade { ok = true };
		this.assert(ok, "if not playing, schedAfterFade should happen immediately");
	}

	test_schedAfterFade {
		var ok = false;
		proxy.fadeTime = 0.1;
		proxy.source = { Silent.ar };
		proxy.schedAfterFade { ok = true };
		0.11.wait;
		this.assert(ok, "if playing, schedAfterFade should happen right after fadeTime");
	}

	test_copy_fadeTimeIsCopied {
        var oldProxy = proxy.fadeTime_(33);
        var newProxy = oldProxy.copy;
        this.assertEquals(oldProxy.fadeTime, newProxy.fadeTime);
    }

	test_copy_awakeIsCopied {
		var oldProxy = proxy.awake_(false);
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.awake, newProxy.awake);
	}

	test_copy_clockIsCopied {
		var oldProxy = proxy.clock_(TempoClock.new(2.182));
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.clock, newProxy.clock);
	}

	test_copy_quantIsCopied {
		var oldProxy = proxy.quant_(4);
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.quant, newProxy.quant);
	}

	test_copy_pauseIsCopied {
		var oldProxy = proxy.pause;
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.paused, newProxy.paused);
	}

	test_copy_sourceIsCopied {
		var oldProxy = proxy.source_({ Silent.ar });
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.source, newProxy.source);
	}

	test_copy_nodeMapIsCopied {
		var oldProxy = proxy.source_({ Silent.ar });
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.nodeMap, newProxy.nodeMap);
	}

	test_copy_independent {
		var copied;
		proxy.source = 1967;
		copied = proxy.copy;
		copied.source = 1968;
		this.assertEquals(proxy.source, 1967, "copying a proxy should return an independent object");
	}


}
