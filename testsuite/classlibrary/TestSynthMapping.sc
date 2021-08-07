/*

make sure audio rate mapping doesn't require ordered nodes

*/

TestSynthAudioMapping : UnitTest {

	var server, mapBus = 120, synth;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
		server.sync;
	}

	tearDown {
		server.quit.remove;
	}

	sendSynthDef { |n1, n2|

		SynthDef(\testNode, {
			var input = \input.ar(0 ! n1);
			var trig = T2A.ar(\trig.tr(0));
			SendReply.ar(trig, "/tr", input);
		}).add;

		SynthDef(\valueNode, {
			var input = \input.ar(-1 ! n2);
			var out = \out.kr(0);
			Out.ar(out, input);
		}).add;

	}

	waitForOSCReply { |callback, timeout = 4|
		var cond = Condition.new;
		var oscRecv = OSCFunc({ |msg| msg.postcs; "----".postln; callback.(msg.drop(3)); cond.unhang; }, "/tr").oneShot;
		SystemClock.sched(timeout, { oscRecv.free; cond.unhang; });
		cond.hang;
	}


	test_map_normal_order {
		var controlSynth, value;
		this.sendSynthDef(1, 1);
		server.sync;
		value = 6561;
		synth = Synth(\testNode);
		controlSynth = Synth.before(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.set(\input, "a" ++ mapBus);
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var val = args.first.round.asInteger;
			this.assertEquals(val, value, "value of synth after mapping should match");
		};
	}

	test_map_reverse_order {
		var controlSynth, value;
		this.sendSynthDef(1, 1);
		server.sync;
		value = 6561;
		synth = Synth(\testNode);
		controlSynth = Synth.after(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.set(\input, "a" ++ mapBus);
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var val = args.first.round.asInteger;
			this.assertEquals(val, value, "value of synth after mapping should match");
		};
	}

	test_map_with_too_many_channels {
		var controlSynth, value, ctlNumChannels, synthNumChannels;
		ctlNumChannels = 5;
		synthNumChannels = 2;
		this.sendSynthDef(synthNumChannels, ctlNumChannels);
		server.sync;
		value = { 6561.rand } ! ctlNumChannels;
		synth = Synth(\testNode);
		controlSynth = Synth.after(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.set(\input, ctlNumChannels.collect { |i| "a" ++ (mapBus + i) });
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var synthVal = args.round.asInteger;
			this.assertEquals(synthVal, value.keep(synthNumChannels), "more channels than available should be still mapped");
		};
	}

	test_map_with_too_few_channels {
		var controlSynth, value, ctlNumChannels, synthNumChannels;
		ctlNumChannels = 2;
		synthNumChannels = 5;
		this.sendSynthDef(synthNumChannels, ctlNumChannels);
		server.sync;
		value = { 6561.rand } ! ctlNumChannels;
		synth = Synth(\testNode);
		controlSynth = Synth.after(synth, \valueNode, [\out, mapBus, \input, value]);
		synth.set(\input, ctlNumChannels.collect { |i| "a" ++ (mapBus + i) });
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var synthVal = args.round.asInteger;
			this.assertEquals(synthVal.keep(ctlNumChannels), value, "fewer channels than available should be still mapped");
		};
	}

	test_map_with_control_rate {
		var controlSynth, value, ctlNumChannels, synthNumChannels, krBus;
		ctlNumChannels = 2;
		synthNumChannels = 5;
		this.sendSynthDef(synthNumChannels, ctlNumChannels);
		krBus = Bus.control(server, ctlNumChannels);
		value = { 6561.rand } ! ctlNumChannels;
		krBus.setn(value);
		server.sync;
		synth = Synth(\testNode);
		synth.set(\input, ctlNumChannels.collect { |i| "c" ++ (krBus.index + i) });
		server.latency.wait;
		synth.set(\trig, 1);
		this.waitForOSCReply { |args|
			var synthVal = args.round.asInteger;
			this.assertEquals(synthVal.keep(ctlNumChannels), value, "fewer channels than available should be still mapped");
		};
	}

}
