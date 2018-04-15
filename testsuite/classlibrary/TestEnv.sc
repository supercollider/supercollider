/*
Env.test
UnitTest.gui
*/
TestEnv : UnitTest {
	var server;

	test_equality {
		var a,b,c;
		a = Env([ 0, 1.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);

		b = Env([ 0, 1.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);
		c = Env([ 0, 2.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);

		this.assert( a == b, "2 identically specified Envs should be equal");

		this.assert( a != c, "2 different Envs should not be equal");

		this.assert( a == a, "an Env should equal itself");
	}

	test_equivalences {

		this.assertArrayFloatEquals(
			Env.cutoff(1, 1, 'exp').asSignal(20).as(Array),
			Env.cutoff(1, 1, 'lin').asSignal(20).asArray.linexp(0,1,-100.dbamp,1),
			"Exponential Env.cutoff should be same as linear Env.cutoff.linexp");
	}

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.remove;
	}

	test_noff_bug334 {
		var synth, n_off_resp, n_end_resp, passed,
		cond = Condition.new,
		cleanup = {
			n_off_resp.free;
			n_end_resp.free;
			server.quit;
		};
		this.bootServer(server);
		synth = {
			// doneAction: 1 should reach end *after* doneAction: 2 envelope ends
			// correct behavior is that '/n_off' is never sent
			EnvGen.kr(Env([0, 1], [0.2]), gate: 0, doneAction: 1);
			EnvGen.kr(Env([0, 1], [0.1]), doneAction: 2);
		}.play(server);
		n_off_resp = OSCFunc({
			cleanup.value;
			passed = false;
			cond.unhang;
		}, '/n_off', server.addr, argTemplate: [synth.nodeID]);
		// responsible for cleaning up this test
		// n_off_resp should NOT fire! so we need another for the stop condition
		n_end_resp = OSCFunc({
			cleanup.value;
			passed = true;
			cond.unhang;
		}, '/n_end', server.addr, argTemplate: [synth.nodeID]);
		cond.hang;
		this.assert(passed, "EnvGen: Initial gate == 0 with doneAction == 1 should not send /n_off");
	}

	// has failed in supernova
	// methodology: synth should play for 0.1 sec if it's working
	// so we will send a trigger just before the envelope ends
	// if the bug occurs, the synth will end almost immediately -- no trigger
	test_nongated_bug3094 {
		var n_end_resp, tr_resp, synth,
		program = [\scsynth], failure,
		cleanup = {
			tr_resp.free;
			n_end_resp.free;
			server.quit;
			Server.scsynth;
		},
		cond = Condition.new;
		// we need to test supernova.
		// In unixy systems, we can try to get the executable path
		// Windows, I don't know
		if(thisProcess.platform.name != "windows" and: {
			"which supernova".unixCmdGetStdOut.size > 0
		}) {
			program = program ++ [\supernova];
		};
		program.do { |p|
			var gotTrigger = false;
			Server.perform(p);
			this.bootServer(server);
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
			if(gotTrigger.not) { failure = failure.add(p) };
		};
		this.assert(failure.size == 0, "EnvGen: Closing gate should not affect non-gated envelope %".format(
			if(failure.size == 0) { "" } {
				"(failed in %)".format(failure)
			}
		));
	}

	/*
	test_initial_value {
		var value = rrand(10.0, 100.0);
		var assert = { |x, curve, selector, label=""|
			this.assert(absdif(x, value) < 0.01,
				"EnvGen.%() with curve '%' (%) should return the first level immediately".format(selector, curve, label)
			)
		};
		this.bootServer;
		Env.shapeNames.keysDo { |curve|
			var env = Env([value, 1, 2], [10, 0.01], curve);
			[\ar, \kr].do { |sel|
				{ EnvGen.perform(sel, env, 1) }.loadToFloatArray(action: { |x| assert.(x[0], curve, sel, "gate = 1") });
					0.1.wait;
				{ |g = 1| EnvGen.perform(sel, env, g) }.loadToFloatArray(action: { |x| assert.(x[0], curve, sel, "gate = gateControl(1)") });
				0.1.wait;
			}
		}
	}
	*/
}

