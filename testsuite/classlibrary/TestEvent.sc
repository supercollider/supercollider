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

	test_parent_proto {



// This shows the order of precedence in an event (higher is overriding lower)
// the tests below make sure the above precedence is valid


/*

                                       +----  (4)  proto of the proto (propro)
                                      /
                   +-- proto  (6) ---+
                  /                   \
                 /                     +----  (3)  parent of the proto (propa)
                /
  event (7) ---+
                \
                 \                     +---- (2)  proto of the parent (papro)
                  \                   /
                   +-- parent  (5) --+
                                      \
                                       +---- (1)  parent of the parent (papa)
*/


		var papa = (x:1);
		var papro = (x:2);
		var propa = (x:4);
		var propro = (x:5);
		var parent = (x:3);
		var proto = (x:6);
		var event = (x:7);

		parent.parent = papa;
		parent.proto = papro;
		proto.parent = propa;
		proto.proto = propro;
		event.parent = parent;
		event.proto = proto;

		this.assert(event[\x] == 7, "value in instance should override value of parent and proto");
		event[\x] = nil;

		this.assert(event[\x] == 6, "value in proto should override value of parent");
		event[\x] == 6;

		proto[\x] = nil;
		this.assert(event[\x] == 5, "value in proto's proto should override proto's parent");

		propro[\x] = nil;
		this.assert(event[\x] == 4, "value in proto's parent should override parent");


		propa[\x] = nil;
		this.assert(event[\x] == 3, "value in parent should override parent's parent and proto");

		parent[\x] = nil;
		this.assert(event[\x] == 2, "value in parent's proto should override value of parent's parent");

		papro[\x] = nil;
		this.assert(event[\x] == 1, "value in parent's parent should be accessible");

		papa[\x] = nil;
		this.assert(event[\x] == nil, "empty parent and proto structure should return nil");

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
	defaultGroupID { ^1 }

}

