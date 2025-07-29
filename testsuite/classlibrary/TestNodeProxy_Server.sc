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
		server.sync;
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

	test_synthDef_isReleased_afterFree {
		var numBefore, numAfter;

		// here we have to make sure that the statusWatcher.numSynthDefs is correct
		// before the experiment, so we force the otherwise regular update ...
		server.sendStatusMsg;
		server.sync;
		numBefore = server.statusWatcher.numSynthDefs;

		// ... then set the proxy source and remove it again ...
		proxy.source = { Silent.ar };
		server.sync;
		proxy.source = nil;

		// ... and also afterwards, we force an update
		server.sync;
		server.sendStatusMsg;
		numAfter = server.statusWatcher.numSynthDefs;

		this.assertEquals(numBefore, numAfter,
			"Removing the  NodeProxy source function should remove SynthDef from server"
		);
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

	test_schedAfterFade_afterQuant {
		var ok = false;
		proxy.source = { Silent.ar };
		1.wait;
		proxy.fadeTime = 0.1;
		proxy.clock = TempoClock.new(1);
		proxy.quant = [1, 0.5];
		0.2.wait;
		proxy.schedAfterFade { ok = true };
		(proxy.fadeTime + 1.5 + proxy.server.latency - 0.2).wait;
		this.assert(ok, "schedAfterFade should happened right after quant and fadeTime");
	}

	test_schedAfterFade_notBeforeQuant {
		var ok = true, earlierThan = 0.05;
		proxy.source = { Silent.ar };
		proxy.fadeTime = 0.1;
		proxy.clock = TempoClock.new(1);
		proxy.quant = 1.0;
		0.2.wait;
		proxy.schedAfterFade { ok = false };
		(proxy.fadeTime + proxy.server.latency + 1.0 - 0.2 - earlierThan).wait;
		this.assert(ok, "schedAfterFade should not happened before quant and fadeTime");
	}

	test_schedAfterFade_cmdPeriod {
		var ok = false;
		proxy.fadeTime = 0.1;
		proxy.source = { Silent.ar };
		proxy.schedAfterFade { ok = true };
		CmdPeriod.run;
		this.assert(ok, "scheduled function should be run at CmdPeriod");
	}

	test_schedAfterFade_cmdPeriod_removed {
		var count = 0;
		proxy.fadeTime = 0.1;
		proxy.source = { Silent.ar };
		proxy.schedAfterFade { count = count + 1 };
		CmdPeriod.run;
		0.11.wait;
		this.assertEquals(count, 1, "scheduled function should be run at CmdPeriod, not twice");
	}

	test_reshaping_freeOldBus_after_fadeTime {
		var oldBus;
		proxy.reshaping = \expanding;
		proxy.source = { Silent.ar.dup(2) };
		proxy.fadeTime = 0.1;
		oldBus = proxy.bus;
		proxy.source = { Silent.ar.dup(3) };
		(proxy.fadeTime + server.latency).wait;
		this.assert(oldBus.index.isNil,
			"When reshaping, the old bus should be free right after fadeTime and server latency"
		);
	}

	test_reshaping_freeOldBus_after_fadeTime_quant {
		var oldBus;
		proxy.reshaping = \expanding;
		proxy.source = { Silent.ar.dup(2) };
		proxy.fadeTime = 0.1;
		proxy.clock = TempoClock.new(10);
		proxy.quant = [1.0, 0.5];
		oldBus = proxy.bus;
		0.01.wait;
		proxy.source = { Silent.ar.dup(3) };
		(proxy.fadeTime + server.latency + 0.5).wait;
		this.assert(oldBus.index.isNil,
			"When reshaping, the old bus should be free right after fadeTime, quant and server latency"
		);
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

	test_stop_object_should_cancel_old_bundle {
		var container;
		proxy.source = { Silent.ar };
		container = proxy.objects.first;
		proxy.free;
		this.assert(container.instVarAt(\prevBundle).isNil, "Pending OSC message should be canceled when proxy synth is freed");
	}

}
