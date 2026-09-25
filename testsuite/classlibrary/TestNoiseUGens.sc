TestNoiseUGens : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_RandSeed_sets_seed {
		var synth;
		var cond = CondVar.new;
		var buffer;
		var ok = false;
		var err;

		var seed = 7777;
		var size = 128;
		// 'drop' b/c empirically, the server side drops one value
		var testValues = Pseed(seed, Pwhite(-1.0, 1.0, inf).drop(1))
		.asStream.nextN(size);
		var serverValues;

		server.bootSync;

		buffer = Buffer.alloc(server, size, 1);
		server.sync;

		try {
			synth = {
				RandSeed.ir(1, seed);
				RecordBuf.ar(WhiteNoise.ar, buffer, loop: 0, doneAction: 2);
				Silent.ar(1)
			}.play(server).onFree {
				ok = true;
				cond.signalAll
			};
			cond.waitFor(0.1, { ok });
			if(ok.not) { Exception("synth timed out").throw };

            ok = false;
			buffer.getn(0, size, { |data|
				serverValues = data;
                ok = true;
				cond.signalAll;
			});
			cond.waitFor(0.1, { ok });
			if(ok.not) { Exception("b_getn failed").throw };
		} { |error|
			if(error.isMemberOf(Exception)) {
				err = error  // my error
			} {
				buffer.free;
				error.throw  // unknown error
			}
		};

		buffer.free;
		this.assert(
			ok and: { testValues == serverValues.as(Array) },
			err.tryPerform(\errorString) ?? { "random values on server were seeded identically to the language" }
		);
	}

	test_RandID_forceArgSetsIDForEveryBlock {
		var synth;
		var cond = CondVar.new;
		var ok = false;
		var osc;
		var countNonMatching;
		var err;
		var seed = 18624;

		server.bootSync;

		synth = {
			var x, y;
			var timer = Line.kr(0, 1, ControlDur.ir * 2, doneAction: 2);
			var seed = 36163;

			#x, y = Array.fill(2, { |i|
				RandID.kr(i, 1);
				RandSeed.kr(1, seed);
				WhiteNoise.ar
			});

			SendReply.kr(Done.kr(timer), '/count', A2K.kr(Integrator.ar((x absdif: y) > 0)));
			Silent.ar(1)
		}.play(server);

		osc = OSCFunc({ |msg|
			countNonMatching = msg[3];
			ok = true;
			cond.signalAll;
		}, '/count', server.addr);
		cond.waitFor(0.1, { ok });

		osc.free;

		if(ok) {
			this.assertEquals(countNonMatching, 0, "Two RNG IDs, seeded equally, should produce 0 non-matching values");
		} {
			this.assert(false, "timed out")
		};
	}
}
