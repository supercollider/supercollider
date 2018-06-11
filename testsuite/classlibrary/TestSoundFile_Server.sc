TestSoundFile_Server : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	test_cue {
		var cueEvent, testEvent, group;

		// pass in Event
		testEvent = (foo: true);
		cueEvent = soundFile.cue(testEvent);
		this.assert(cueEvent[\foo], "Cue should take an Event as first argument");

		// playNow argument
		cueEvent = soundFile.cue((), true);
		0.5.wait;
		this.assert(cueEvent.isRunning, "Event should play immediately after cueing");
		cueEvent.stop;

		// Test settable controls
		group = Group.new(server);
		testEvent = (bufferSize: 262144, firstFrame: 100, lastFrame: 200, out: 20, server: server, group: group, addAction: 1, amp: 0);
		cueEvent = soundFile.cue(testEvent);
		testEvent.keysDo { |key|
			this.assertEquals(cueEvent[key], testEvent[key], "Control '%' was correclty set".format(key))
		};

		// closeWhenDone argument
		soundFile.openRead;
		soundFile.cue((lastFrame: 10), true, true);
		0.5.wait;
		this.assert(soundFile.isOpen.not, "SoundFile should close when finished playing");

		// test cleanup
		Buffer.freeAll;
		server.quit.remove;
	}

}
