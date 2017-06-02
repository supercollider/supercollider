/*
TestEvent.run
UnitTest.gui
*/
TestEvent : UnitTest {

	var prevMsg, prevLatency;

	test_equality {

		var a,b,c, x, y, keys, values;

		a = (a: 100, b: 200, c: [1, 2, 3]);
		b = (a: 100, b: 200, c: [1, 2, 3]);
		c = (a: 100, b: 200, c: [1, 2, 4]);

		this.assert( a == b, "2 identically specified Events should be equal");

		this.assert( a != c, "2 different Events should not be equal");

		this.assert( a == a, "an Events should equal itself");

		x = ().putPairs([{ |i| ("key_" ++ i).asSymbol }.dup(80), 0].flop.flat);
		keys = x.keys.asArray;
		values = keys.collect { |key| x.at(key) };

		this.assert(values.includes(nil).not, "an Event responds to all its keys");

		x.use {
			y = ().putPairs(keys.envirPairs);
		};

		this.assert(x == y, "an Event remains invariant under transformation to envirPairs");

	}

	test_server_messages {
		// type note
		var event = (
			type: \note,
			instrument: \xxx_test,
			server: this,
			note: 0,
			octave: 6,
			ctranspose: 1,
			zzzz: 0,
			latency: 0,
			finish: { finishTest = true }
		);
		var i, msg1, finishTest = false;
		var isPlaying;

		SynthDef(\xxx_test, { |freq, zzzz, gate = 1|

		}).add;

		this.assert(SynthDescLib.global.at(\xxx_test).canFreeSynth.not,
			"SynthDesc detects that Synth can't free itself");

		SynthDef(\xxx_test, { |freq, zzzz, gate = 1|
			EnvGen.kr(Env.asr, gate, doneAction:2)
		}).add;

		this.assert(SynthDescLib.global.at(\xxx_test).canFreeSynth,
			"SynthDesc detects that Synth can free itself");

		event.play;
		0.5.wait;

		msg1 = prevMsg[0];
		i = msg1.indexOf(\freq);
		this.assert(msg1[i+1].cpsmidi == 73, "note 0 octave 6 ctranspose 1 equals midinote 73");
		this.assert(msg1[4] == 1, "synth created in root node");
		this.assert(msg1.includes(\zzzz), "SynthDesc provides parameters to send in message");
		this.assert(event[\hasGate] == true, "Event detects gate in SynthDesc");
		this.assert(finishTest, "Event evaluates finish action");
		this.assert(prevLatency == 0,
			"latency specified in the event should override server latency");
		prevMsg = nil;
		prevLatency = nil;
	}

	sendMsg { |... args| prevMsg = prevMsg.add(args) }
	sendBundle { |latency, args| prevMsg = prevMsg.add(args); prevLatency = latency; }

	nextNodeID { ^-1 }
	latency { ^0.2 }

}

