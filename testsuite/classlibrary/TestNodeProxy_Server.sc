TestNodeProxy_Server : UnitTest {

	var server, proxy;

	setUp {
		server = Server(this.class.name);
		server.options.numWireBufs = 2048;
		proxy = NodeProxy(server);
		server.bootSync;
	}

	tearDown {
		proxy.clear;
		server.quit;
		server.remove;
	}

	test_send_loaded {
		proxy.source = { Silent.ar };
		proxy.send;
		this.assertEquals(proxy.loaded, true, "After sending, NodeProxy should be loaded");
	}

	test_send_isPlaying {
		proxy.source = { Silent.ar };
		proxy.send;
		this.assertEquals(proxy.isPlaying, true, "After sending, NodeProxy should be playing");
	}

	test_rebuild {
		var build;

		proxy.source = { build = true; Silent.ar };
		// set build to false in order to check that it gets set back to true after rebuild
		build = false;
		proxy.rebuild;
		this.assertEquals(build, true, "Calling rebuild should build SynthDef again");
	}

	test_rebuild_loaded {
		proxy.source = { Silent.ar };
		proxy.rebuild;
		this.assertEquals(proxy.loaded, true, "After rebuild, NodeProxy should be loaded");
	}

	test_send_synthDef_rebuild {
		var build;

		proxy.source = { build = true; Silent.ar };
		// set build to false in order to check whether it stays false after calling send
		build = false;
		proxy.send;
		this.assertEquals(build, false, "SynthDef should not be rebuilt when calling send");
	}

	test_loaded_after_quit {
		proxy.send;
		server.quit;
		this.assertEquals(proxy.loaded, false, "NodeProxy should not be loaded after server quit");
	}

	test_send_after_quit {
		proxy.send;
		server.quit;
		proxy.send;
		this.assertEquals(proxy.loaded, false, "After server quit, sending should not set node as loaded");
	}

	test_rebuild_after_quit {
		var build;

		proxy.source = { build = true; Silent.ar };
		server.quit;
		proxy.rebuild;
		this.assertEquals(build, true, "After server quit, rebuilding NodeProxy should rebuild SynthDef");
	}

	test_reshaping_nil_numChannels_expansion {
		proxy.ar(2);
		proxy.reshaping = nil;
		proxy.source = { Silent.ar.dup(8) };
		this.assertEquals(proxy.numChannels, 2, "When reshaping is nil, NodeProxy's number of channels should NOT be able to expand");
	}

	test_reshaping_nil_numChannels_contraction {
		proxy.ar(8);
		proxy.reshaping = nil;
		proxy.source = { Silent.ar.dup(2) };
		this.assertEquals(proxy.numChannels, 8, "When reshaping is nil, NodeProxy's number of channels should NOT be able to contract");
	}

	test_reshaping_elastic_numChannels_expansion {
		proxy.ar(2);
		proxy.reshaping = \elastic;
		proxy.source = { Silent.ar.dup(8) };
		this.assertEquals(proxy.numChannels, 8, "When reshaping is elastic, NodeProxy's number of channels should be able to expand");
	}

	test_reshaping_elastic_numChannels_contraction {
		proxy.ar(8);
		proxy.reshaping = \elastic;
		proxy.source = { Silent.ar.dup(2) };
		this.assertEquals(proxy.numChannels, 2, "When reshaping is elastic, NodeProxy's number of channels should be able to contract");
	}

	test_reshaping_expanding_numChannels_expansion {
		proxy.ar(2);
		proxy.reshaping = \expanding;
		proxy.source = { Silent.ar.dup(8) };
		this.assertEquals(proxy.numChannels, 8, "When reshaping is expanding, NodeProxy's number of channels should be able to expand");
	}

	test_reshaping_expanding_numChannels_contraction {
		proxy.ar(8);
		proxy.reshaping = \expanding;
		proxy.source = { Silent.ar.dup(2) };
		this.assertEquals(proxy.numChannels, 8, "When reshaping is expanding, NodeProxy's number of channels should NOT be able to contract");
	}

	test_clear_fadeTime {
		var clearTime = 0.1;

		proxy.fadeTime = 2;
		proxy.reshaping = \elastic;
		proxy.clear(clearTime);
		clearTime.wait;

		this.assertEquals(proxy.isNeutral, true, "After clearing with a fadeTime, NodeProxy should be neutral");
	}

	test_synthDefControl_build {
		proxy.source = { Silent.ar };
		this.assertEquals(proxy.objects.first.hasFadeTimeControl, true, "Functions should register their fadeTime control");
	}

	test_control_fade {
		var result, timeout;
		var cond = Condition.new;
		var fadeTime = 0.2;
		var waitTime = (fadeTime + (server.latency * 2) + 1e-2);

		proxy.source = { DC.kr(2000) };
		proxy.fadeTime = fadeTime;
		proxy.source = 440;
		waitTime.wait;

		OSCFunc({ cond.unhang }, '/c_set');
		timeout = fork{ 1.wait; cond.unhang };
		proxy.bus.get({ |val| result = val });
		cond.hang;
		timeout.stop;

		this.assertEquals(result, proxy.source, "After the crossfade from a ugen function to a value, the bus should have the correct value");
	}

}
