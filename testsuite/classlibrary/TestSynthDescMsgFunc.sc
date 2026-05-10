TestSynthDescMsgFunc : UnitTest {
	test_SynthDesc_normal_SynthDef_event_args {
		var event, args;
		SynthDef(\normal_test_args, {
			arg out = 0, gate = 1, freq = 440, ffreq = 2000, rq = 0.2, amp = 0.1, t_trig = 1;
			// note, we don't need actual DSP, only checking args
			Out.ar(0, DC.ar(0))
		}).add;

		event = (out: 4, gate: 1, freq: 800, ffreq: 8234, amp: 0.2, trig: 1,
			t_trig: 2, undefined: 600);
		event.use {
			args = SynthDescLib.at(\normal_test_args).msgFunc.valueEnvir;
		};

		this.assert(
			this.unorderedPairsEqual(
				args,
				[out: 4, freq: 800, ffreq: 8234, amp: 0.2, t_trig: 1]
			),
			"Arg list from event should include specified args, skip omitted and undefined"
		)
	}

	test_SynthDesc_300_args_from_event {
		var event, indices, argsFromDesc, inputArgs;
		SynthDef('300_controls', {
			var ctl = Control.names(
				Array.fill(300, { |i| ("ctl" ++ i).asSymbol })
			).kr(Array.fill(300, { |i| i/300 }));
			Out.ar(0, DC.ar(0))
		}).add;

		event = ();
		indices = (0, 7 .. 299);
		inputArgs = Array(indices.size * 2);
		indices.do { |i|
			var x = 1.0.rand2;
			event[("ctl" ++ i).asSymbol] = x;
			inputArgs.add(("ctl" ++ i).asSymbol).add(x);
		};
		event.use {
			argsFromDesc = SynthDescLib.at('300_controls').msgFunc.valueEnvir;
		};

		this.assert(
			this.unorderedPairsEqual(
				argsFromDesc,
				inputArgs
			),
			"Arg list from event should include specified args, skip omitted and undefined"
		)
	}

	test_SynthDesc_large_control_count_with_arrayed_controls {
		var event, indices, argsFromDesc, inputArgs;
		var numPerCtl = Array.fill(300, { rrand(1, 3) });

		SynthDef(\large_with_arrays, {
			var ctl = Control.names(
				Array.fill(300, { |i| ("ctl" ++ i).asSymbol })
			).kr(Array.fill(300, { |i| Array.fill(numPerCtl[i], 0) }));
			Out.ar(0, DC.ar(0))
		}).add;

		event = ();
		indices = (0, 7 .. 299);
		inputArgs = Array(indices.size * 2);
		indices.do { |i|
			var x = Array.fill(numPerCtl[i], { 1.0.rand2 });
			event[("ctl" ++ i).asSymbol] = x;
			inputArgs.add(("ctl" ++ i).asSymbol).add(x);
		};
		event.use {
			argsFromDesc = SynthDescLib.at(\large_with_arrays).msgFunc.valueEnvir;
		};

		this.assert(
			this.unorderedPairsEqual(
				argsFromDesc,
				inputArgs
			),
			"Arg list from event should include specified args, skip omitted and undefined"
		)
	}

	test_SynthDesc_msgFunc_for_empty_arg_list_is_empty_array {
		// by default, 'gate' is excluded.
		// SynthDesc should recognizes that there's nothing else to include,
		// and produce an empty-array msgFunc
		SynthDef(\testempty, { |gate = 1|
			Out.kr(1000, gate)
		}).add;

		this.assertEquals(
			SynthDescLib.at(\testempty).msgFunc.value,
			Array.new,
			"The msgFunc for a SynthDef with no eligible controls should return an empty array"
		);
	}

	unorderedPairsEqual { |a, b|
		var aDict = a.asDict(class: IdentityDictionary);
		var bDict = b.asDict(class: IdentityDictionary);
		if((aDict.keys symmetricDifference: bDict.keys).size > 0) {
			^false  // one contains items not present in the other
		};
		// having already established they both have the same keys,
		// we can check unidirectionally
		aDict.keysValuesDo { |k, v|
			if(bDict[k] != v) { ^false }
		};
		^true
	}
}
