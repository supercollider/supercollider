TestMixedBundle_Server : UnitTest {

	var server, bundle;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
		bundle = MixedBundle.new;
		this.makeDefs(100);
	}

	tearDown {
		server.quit;
		server.remove;
	}

	// helper method, not a test
	makeDefs { |numDefs|
		numDefs.do { |i|
			var def = SynthDef("test_" ++ i, { Out.ar(0, Silent.ar) });
			bundle.addPrepare(['/d_recv', def.asBytes])
		}
	}

	test_addFunction {
		var evaluated, timeout;
		var cond = Condition.new;

		bundle.addFunction({ evaluated = true; cond.unhang });

		timeout = fork { 3.wait; cond.unhang };
		bundle.send(server);
		cond.hang;
		timeout.stop;

		this.assertEquals(evaluated, true, "Sending the bundle should call functions");
	}

	test_onSend {
		var evaluated, timeout;
		var cond = Condition.new;

		bundle.onSend({ evaluated = true; cond.unhang });

		timeout = fork { 3.wait; cond.unhang };
		bundle.send(server);
		cond.hang;
		timeout.stop;

		this.assertEquals(evaluated, true, "Sending the bundle should call sendFunctions");
	}

	test_send_synth_bundle {
		var timeout;
		var cond = Condition.new;
		var synth = Synth.basicNew(\default, server);

		synth.register;
		bundle.add(synth.newMsg);

		OSCFunc({ cond.unhang }, '/n_go').oneShot;
		timeout = fork { 3.wait; cond.unhang };
		bundle.send(server);
		cond.hang;
		timeout.stop;

		this.assertEquals(synth.isPlaying, true, "Synth should be playing after sending bundle");
	}

}
