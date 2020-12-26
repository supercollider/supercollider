/*

make sure audio rate mapping doesn't require ordered nodes

*/

TestSynthAudioMapping : UnitTest {

	var server, mapBus = 120, synth;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
		server.sync;
		this.sendSynthDef;
		server.sync;

	}

	tearDown {
		server.quit.remove;
	}

	sendSynthDef {

		SynthDef(\testNode, {
			var input = \input.ar(0);
			var trig = T2A.ar(\trig.tr(0));
			SendTrig.ar(trig, 0, input);
		}).add;

		SynthDef(\valueNode, {
			var input = \input.ar(-1);
			var out = \out.kr(0);
			Out.ar(out, input);
		}).add;

	}

	waitForOSCReply { |callback, timeout = 4|
		var oscRecv = OSCFunc({ |msg| callback.(msg.drop(1)); }, "/tr").oneShot;
		SystemClock.sched(timeout, { oscRecv.free });
	}

	test_noop {
		synth = Synth(\testNode);
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var val = args.first;
			this.assertEqualValue(val, -1, "default value of synth before mapping should be -1");
		};
	}

	test_map_normal_order {
		var controlSynth, value;
		value = 6561;
		synth = Synth(\testNode);
		controlSynth = Synth.before(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.map(\input, mapBus);
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var val = args.first;
			this.assertEqualValue(val, value, "value of synth after mapping should match");
		};
	}

	test_map_reverse_order {
		var controlSynth, value;
		value = 6561;
		synth = Synth(\testNode);
		controlSynth = Synth.after(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.map(\input, mapBus);
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var val = args.first;
			this.assertEqualValue(val, value, "value of synth after mapping should match");
		};
	}

}
