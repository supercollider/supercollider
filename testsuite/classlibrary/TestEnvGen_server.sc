TestEnvGen_server : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		if(server.serverRunning) { server.quit };
		server.remove;
	}

	test_zero_gate_n_off_not_sent {
		var synth, n_off_resp, n_end_resp, passed;
		var cond = Condition.new;
		var cleanup = {
			n_off_resp.free;
			n_end_resp.free;
		};

		server.bootSync;
		synth = {
			// doneAction: 1 should reach end *after* doneAction: 2 envelope ends
			// correct behavior is that '/n_off' is never sent
			EnvGen.kr(Env([0, 1], [0.2]), gate: 0, doneAction: 1);
			EnvGen.kr(Env([0, 1], [0.1]), doneAction: 2);
		}.play(server);

		// EnvGen is initialized by setting the state to the end of the Env,
		// which fires the doneAction on initialization. So make sure that
		// initialization has happened before adding these reponders.
		server.sync;

		n_off_resp = OSCFunc({
			cleanup.value;
			passed = false;
			cond.unhang;
		}, '/n_off', server.addr, argTemplate: [synth.nodeID]);

		// responsible for cleaning up this test
		// n_off_resp should NOT fire! so we need this to unhang the condition
		n_end_resp = OSCFunc({
			cleanup.value;
			passed = true;
			cond.unhang;
		}, '/n_end', server.addr, argTemplate: [synth.nodeID]);

		cond.hang;
		this.assert(passed, "Initial gate == 0 with doneAction == 1 should not send /n_off");
	}

	// has failed in supernova
	// methodology: synth should play for 0.1 sec if it's working
	// so we will send a trigger just before the envelope ends
	// if the bug occurs, the synth will end almost immediately -- no trigger
	test_trig_gate_nonsustain_env {
		var n_end_resp, tr_resp, synth;
		var program = [\scsynth];
		var cond = Condition.new;
		var cleanup = {
			tr_resp.free;
			n_end_resp.free;
			server.quit;
			Server.scsynth;
		};

		// we need to test supernova.
		// In unixy systems, we can try to get the executable path
		// Windows, I don't know
		if(
			thisProcess.platform.name != "windows"
			and: { "which supernova".unixCmdGetStdOut.size > 0 }
		) {
			program = program ++ [\supernova];
		};

		program.do { |p|
			var gotTrigger = false;

			Server.perform(p);
			server.bootSync;

			synth = {
				var gate = Impulse.kr(0);
				SendTrig.kr(TDelay.kr(gate, 0.09), 0, 1);
				EnvGen.kr(Env([0, 1], [0.1]), gate, doneAction: 2);
			}.play(server);

			tr_resp = OSCFunc({ |msg|
				gotTrigger = true;
			}, '/tr', server.addr, argTemplate: [synth.nodeID]);

			n_end_resp = OSCFunc({ |msg|
				cleanup.value;
				cond.unhang;
			}, '/n_end', server.addr, argTemplate: [synth.nodeID]);

			cond.hang;
			this.assert(gotTrigger, "Closing gate should not affect non-sustaining envelopes when using %".format(p));
		};
	}

	// forced release tests
	test_neg_gate_ignores_multinode_release {
		var synth, n_failed_resp, n_end_resp, passed;
		var cond = Condition.new;
		var cleanup = {
			n_failed_resp.free;
			n_end_resp.free;
		};

		server.bootSync;
		synth = {|gate=1|
			var env = EnvGen.kr(Env([0,0.1,1,0], [0.1], releaseNode:1), gate, doneAction: 2);
			SendReply.kr(env>0.1, '/failed')
		}.play(server);

		n_failed_resp = OSCFunc({
			cleanup.value;
			passed = false;
			cond.unhang;
		}, '/failed', server.addr, argTemplate: [synth.nodeID]);

		// responsible for cleaning up this test
		// n_failed_resp should NOT fire! so we need this to unhang the condition
		n_end_resp = OSCFunc({
			cleanup.value;
			passed = true;
			cond.unhang;
		}, '/n_end', server.addr, argTemplate: [synth.nodeID]);

		server.sync;
		synth.release(0.1);

		cond.hang;
		this.assert(passed, "gate < 0 should ignore multi-node release");
	}

	test_forced_release_respects_env_curve {
		var synth, n_failed_resp, n_end_resp, passed;
		var cond = Condition.new;
		var cleanup = {
			n_failed_resp.free;
			n_end_resp.free;
		};

		server.bootSync;
		synth = {|gate=1|
			var env = EnvGen.kr(Env([0,1,0], [0.1], \step, releaseNode: 1), gate, doneAction: 2);
			SendReply.kr((env > 0.1) * (env < 0.9), '/failed')
		}.play(server);

		n_failed_resp = OSCFunc({
			cleanup.value;
			passed = false;
			cond.unhang;
		}, '/failed', server.addr, argTemplate: [synth.nodeID]);

		// responsible for cleaning up this test
		// n_failed_resp should NOT fire! so we need this to unhang the condition
		n_end_resp = OSCFunc({
			cleanup.value;
			passed = true;
			cond.unhang;
		}, '/n_end', server.addr, argTemplate: [synth.nodeID]);

		server.sync;
		synth.release(0.1);

		cond.hang;
		this.assert(passed, "gate < 0 should respect env's last node curve");
	}

	// issue #5104
	test_shapeHold_setEndValue {
		var env = Env(curve: [\hold, \lin]);
		var condvar = CondVar();
		var result = [];
		var timeScale = 0.01;

		server.bootSync;

		{ EnvGen.kr(env, timeScale: timeScale, doneAction:2) }.loadToFloatArray(env.duration * timeScale, server){ |values|
			result = values;
			condvar.signalOne;
		};

		condvar.waitFor(5);

		this.assert(result.any { |v| v > 0 }, "Env(curves: [\hold, \lin]) should not be rendered as silent");
	}

}
