//# VERSION 3.16
TestSoundFile_Server : UnitTest {

	var server, soundFile;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);

		soundFile = SoundFile(ExampleFiles.child);
	}

	tearDown {
		soundFile.close;
		Buffer.freeAll;
		server.quitSync.remove;
	}

	test_cue_passInEvent {
		var condition = Condition.new;

		var testEvent = (foo: true);
		var cueEvent = soundFile.cue(testEvent);
		this.assert(cueEvent[\foo], "Cue should take an Event as first argument");
	}

	test_cue_playNow {
		var condition = Condition.new;

		OSCdef(\isPlaying, { condition.unhang }, '/n_go', server.addr);
		var cueEvent = soundFile.cue((), true);
		var timeout = fork { 3.wait; condition.unhang };
		condition.hang;
		this.assert(cueEvent.isRunning, "Event should play immediately after cueing");
		timeout.stop;
		OSCdef(\isPlaying).free;
		cueEvent.stop;
	}

	test_cue_eventConfiguration {
		var condition = Condition.new;

		var group = Group.new(server);
		var testEvent = (bufferSize: 262144, firstFrame: 100, lastFrame: 200, out: 20, server: server, group: group, addAction: 1, amp: 0);
		var cueEvent = soundFile.cue(testEvent);
		testEvent.keysDo { |key|
			this.assertEquals(cueEvent[key], testEvent[key], "Control '%' was correclty set".format(key))
		};
	}

}
