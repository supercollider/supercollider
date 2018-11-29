TestDoneActions : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
		server.notify;
		server.sync;

		SynthDef(this.class.name, { |doneAction|
			Line.kr(0, 0, 0.01, doneAction:doneAction)
		}).send(server);

		server.sync;
	}

	tearDown {

		server.quit.remove;

	}

	test_freeSelfAndResumeNext {
		var synths, allPlaying, allEnded;

		synths = Array.fill(4, {
			Synth.newPaused(
				this.class.name,
				[\doneAction, Done.freeSelfResumeNext],
				server,
				addAction:\addToTail
			).register
		});

		0.2.wait;
		allPlaying = synths.every { |x| x.isPlaying };

		synths.first.run(true);

		0.2.wait;
		allEnded = synths.every { |x| x.isPlaying.not };

		this.assert(allPlaying, "the chain of synths should first be playing");
		this.assert(allEnded, "the chain of synths should have freed itself");
	}

}
