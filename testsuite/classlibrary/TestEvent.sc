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

	test_event_addParentType {
		var event, noError = true;
		Event.addEventType(\testType, {
			~calculated = ~given;
		}, (given: 1));
		// must play to attach parent event
		event = (type: \testType).play;
		this.assert(event[\calculated] == 1, "parentType values should be accessible during event play");
	}

	test_server_message_head_type_grain {
		this.assertEqualServerMessage(\grain, [9, \default, -1, 0,  this.defaultGroupID])
	}

	test_server_message_head_type_on {
		this.assertEqualServerMessage(\on, [9, \default, -1, 0,  this.defaultGroupID])
	}

	test_server_message_head_type_off {
		this.assertEqualServerMessage(\off, [15, 77, \gate, 0], (id:77))
	}

	test_server_message_head_type_set {
		this.assertEqualServerMessage(\set, [15, 77], (id:77))
	}


	test_server_message_head_type_kill {
		this.assertEqualServerMessage(\kill, [\n_free, 77], (id:77))
	}


	test_server_message_head_type_group {
		this.assertEqualServerMessage(\group, [21, -1, 0,  this.defaultGroupID])
	}

	test_server_message_head_type_parGroup {
		this.assertEqualServerMessage(\parGroup, [ 63, -1, 0, this.defaultGroupID])
	}

	test_server_message_head_type_bus {
		this.assertEqualServerMessage(\bus, [\c_setn, 0, 0])
	}

	test_server_message_head_type_fadeBus {
		this.assertEqualServerMessage(\fadeBus, [9, "system_setbus_control_0", -1, 1, this.defaultGroupID])
	}

	test_server_message_head_type_gen {
		this.assertEqualServerMessage(\gen, [\b_gen, 0, \sine1, 7, 1])
	}

	test_server_message_head_type_load {
		this.assertEqualServerMessage(\load, [\b_allocRead, 0, "hello/path", 0, 0], (filename: "hello/path"))
	}

	test_server_message_head_type_read {
		this.assertEqualServerMessage(\read, [ \b_read, 0, "hello/path", 0, 0], (filename: "hello/path"))
	}

	test_server_message_head_type_alloc {
		this.assertEqualServerMessage(\alloc, [\b_alloc, 0, 0, 1])
	}

	test_server_message_head_type_free {
		this.assertEqualServerMessage(\free, [\b_free, 0])
	}

	test_server_message_head_type_monoOff {
		this.assertEqualServerMessage(\monoOff, [ 15, 77, \gate, 0], (id: 77))
	}

	test_server_message_head_type_monoSet {
		this.assertEqualServerMessage(\monoSet, [15, 77, "freq", 123],  (id: 77, msgFunc: { |freq| ["freq", freq] }, freq: 123))
	}

	// arguable: maybe the group should be teh default group per default.
	test_server_message_head_type_monoNote {
		this.assertEqualServerMessage(\monoNote, [9, \default, -1, 0, this.defaultGroupID], (group: this.defaultGroupID))
	}



	assertEqualServerMessage {  |type, shouldBe, parameters|
		var message;
		var event = (
			type: type,
			server: this,
			instrument: \default,
		).putAll(parameters ? ());
		event.play;
		0.01.wait;
		message = prevMsg[0];
		this.assertEquals(
			message.keep(shouldBe.size),
			shouldBe,
			"event type % should generate an OSC message that begins like this.".format(type)
		);
		this.cleanUpMessages;
	}

	test_server_messages_type_note {
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
		this.cleanUpMessages;
	}

	cleanUpMessages {
		prevMsg = nil;
		prevLatency = nil;
	}

	sendMsg { |... args|
		prevMsg = prevMsg.add(args)
	}
	sendBundle { |latency, args|
		prevMsg = prevMsg.add(args);
		prevLatency = latency;
	}

	nextNodeID { ^-1 }

	latency { ^Server.default.latency }
	defaultGroupID { ^Server.default.defaultGroupID }
	defaultGroup { ^Server.default.defaultGroup }


}
