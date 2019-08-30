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

}
