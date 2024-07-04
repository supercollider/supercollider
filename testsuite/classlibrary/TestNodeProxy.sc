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

	test_source_neutral_proxy_elastic {
		var otherProxy = NodeProxy.new(server);
		proxy.reshaping = \elastic;
		proxy.initBus(\audio, 2);
		proxy.source = otherProxy;
		this.assert(proxy.rate == otherProxy.rate, "proxy as source should initialise receiver");
		otherProxy.clear;
	}

	test_asCode_basic {
		var codeString = "a = NodeProxy.new(Server.fromName('TestNodeProxy'));\n";

		this.assertEquals(proxy.asCode, codeString,
			"asCode-posting basic nodeproxy should post valid source code.");
	}

	test_asCode_single {
		var codeString = "a = NodeProxy.new(Server.fromName('TestNodeProxy')).source_({ DC.ar });\n";
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
		"a = NodeProxy.new(Server.fromName('TestNodeProxy'));\n"
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
		"a = NodeProxy.new(Server.fromName('TestNodeProxy'));\n"
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
		var oldProxy = proxy.source_({ Silent.ar }).set(\freq, 7817);
		var newProxy = oldProxy.copy;

		this.assertEquals(oldProxy.nodeMap.at(\freq), newProxy.nodeMap.at(\freq));
	}

	test_copy_independent {
		var copied;
		proxy.source = 1967;
		copied = proxy.copy;
		copied.source = 1968;
		this.assertEquals(proxy.source, 1967, "copying a proxy should return an independent object");
	}

	test_control_fade {
		var server = Server(this.class.name);
		var result, proxy, timeout;
		var cond = Condition.new;
		var fadeTime = 0.1;
		var waitTime = (fadeTime + (server.latency * 2) + 1e-2);
		var resp;

		server.bootSync;

		proxy = NodeProxy(server);
		proxy.source = { DC.kr(2000) };
		proxy.fadeTime = fadeTime;

		proxy.source = 440;
		waitTime.wait;

		resp = OSCFunc({ cond.unhang; resp.free; }, '/c_set');
		timeout = fork { 1.wait; cond.unhang };
		proxy.bus.get({ |val| result = val });
		cond.hang;
		timeout.stop;
		resp.free;

		this.assertFloatEquals(result, proxy.source, "after the crossfade from a ugen function to a value the bus should have this value");

		server.quit;
		server.remove;
	}

	test_map_rates {

		var p = ProxySpace.new.use {

			~out = { \in.ar(0 ! 2) };
			~osc = { |freq = 440| Saw.ar(freq).dup };
			~osc <>> ~out;
			this.assertEquals(~out.controlNames.detect { |cn| cn.name == \in }.rate, \audio,
				"report the correct controlNames rates for audio rate proxies"
			);
		};

		p.clear;

	}

	test_buildProxy_inside {
		var buildProxyFromInside;
		proxy.source = { buildProxyFromInside = NodeProxy.buildProxy };
		this.assertEquals(buildProxyFromInside, proxy, "From inside the source, buildProxy should be the current proxy");
	}
	test_buildProxy_outside {
		var buildProxyFromOutside;
		proxy.source = { 0.0 };
		this.assert(buildProxyFromOutside.isNil, "From theoutside, buildProxy should be nil");
	}

}


TestNodeProxyBusMapping : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
		server.latency = 0.1;
		this.bootServer(server);
		server.sync;
	}


	tearDown {
		server.quit.remove;
	}

	test_audiorate_mapping {
		var proxy, inputProxy;
		var synthValues, controlValues, defaultValues;

		defaultValues = [-1.0, -2.0];
		controlValues = [162.0, 54.0];

		proxy = NodeProxy(server, \audio);
		proxy.reshaping = \elastic;
		proxy.fadeTime = 0;

		proxy.source = {
			var in = \in.ar(defaultValues);
			A2K.kr(in);
		};

		inputProxy = NodeProxy(server, \audio);
		inputProxy.source = {
			DC.ar(controlValues)
		};

		0.2.wait;

		this.assert(proxy.rate == \control, "proxy should have initialized itself to control rate");

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues, defaultValues, "before mapping, synth values should be default values");

		proxy <<>.in inputProxy;

		0.2.wait;

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues, controlValues, "after mapping, synth should return mapped values");


	}

	test_audiorate_mapping_elasticity {
		var proxy, inputProxy;
		var synthValues, controlValues, defaultValues;

		defaultValues = [-1.0, -2.0, -3.0];
		controlValues = [162.0, 54.0, 45.0, 87.0];

		proxy = NodeProxy(server, \audio, 7);
		proxy.reshaping = \elastic;
		proxy.fadeTime = 0.001;

		proxy.source = {
			var in = \in.ar(defaultValues);
			A2K.kr(in);
		};

		inputProxy = NodeProxy(server, \audio, 2);
		inputProxy.reshaping = \elastic;
		inputProxy.fadeTime = 0.001;
		inputProxy.source = {
			DC.ar(controlValues)
		};

		0.2.wait;

		this.assertEquals(proxy.rate, \control, "proxy should have initialized itself to control rate");
		this.assertEquals(proxy.numChannels, defaultValues.size, "proxy should have initialized itself to the correct number of channels");

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues.round, defaultValues.round, "before mapping, synth values should be default values");

		proxy <<>.in inputProxy;

		0.2.wait;

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues.round, controlValues.keep(synthValues.size).round, "after mapping, synth should return mapped values");
	}
}

TestNodeProxySeti : UnitTest {
	var proxy, server;
	setUp {
		server = Server(this.class.name);
		proxy = NodeProxy.audio(server, 5);
		proxy.source = { SinOsc.ar(\freq.kr(200!5), \phase.kr(0!5)) };

	}

	tearDown {
		proxy.clear;
		server.remove;
	}

	test_seti {
		var keysValues;
		proxy.seti(\freq, 2, 212, \phase, 1, 0.5pi);
		keysValues = proxy.getKeysValues;
		this.assertEquals(keysValues[0][1][2], 212, "arg 'freq' should have been set to 212 for the third channel in NodeProxy 'proxy'.");
		this.assertEquals(keysValues[1][1][1], 0.5pi, "arg 'phase' should have been set to 0.5pi for the second channel in NodeProxy 'proxy'.");
	}

	test_seti_multi {
		var keysValues;
		proxy.seti(\freq, [1, 3], [212, 328]);
		keysValues = proxy.getKeysValues;
		this.assertEquals(keysValues[0][1], [200, 212, 200, 328, 200], "the 'freq' arg array should have been set to [200, 212, 212, 328, 200] in NodeProxy 'proxy'.");
	}

	test_seti_wrapAt {
		var keysValues;
		proxy.seti(\freq, [1, 23, 57], [345, 145]);
		keysValues = proxy.getKeysValues;
		this.assertEquals(keysValues[0][1], [200, 345, 345, 145, 200], "the 'freq' arg array should have been set to [200, 345, 345, 145, 200] in NodeProxy 'proxy'.");
	}

	test_seti_nodeMap {
		var controlProxy, keysValues;
		controlProxy = NodeProxy.control(server, 1);
		controlProxy.source = { DC.kr };
		proxy.seti(\freq, 2, controlProxy);
		keysValues = proxy.getKeysValues;
		this.assertEquals(keysValues[0][1], [200, 200, controlProxy, 200, 200], "the third slot in the 'freq' arg array should have been set to NodeProxy.control(server, 1)");
		controlProxy.clear;
	}
}
