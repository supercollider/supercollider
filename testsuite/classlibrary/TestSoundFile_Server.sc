TestSoundFile_Server : UnitTest {

	var server, soundFile;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);

		soundFile = SoundFile(Platform.resourceDir +/+ "sounds/a11wlk01.wav");
	}

	tearDown {
		soundFile.close;
		Buffer.freeAll;
		server.quit.remove;
	}

	test_cue_passInEvent {
		var cueEvent, testEvent, condition = Condition.new;

		testEvent = (foo: true);
		cueEvent = soundFile.cue(testEvent);
		this.assert(cueEvent[\foo], "Cue should take an Event as first argument");
	}

	test_cue_playNow {
		var cueEvent, condition = Condition.new;

		OSCFunc({ condition.unhang }, '/n_go', server.addr);
		cueEvent = soundFile.cue((), true);
		fork { 3.wait; condition.unhang };
		condition.hang;
		this.assert(cueEvent.isRunning, "Event should play immediately after cueing");
		cueEvent.stop;
	}

	test_cue_eventConfiguration {
		var cueEvent, testEvent, group, condition = Condition.new;

		group = Group.new(server);
		testEvent = (bufferSize: 262144, firstFrame: 100, lastFrame: 200, out: 20, server: server, group: group, addAction: 1, amp: 0);
		cueEvent = soundFile.cue(testEvent);
		testEvent.keysDo { |key|
			this.assertEquals(cueEvent[key], testEvent[key], "Control '%' was correclty set".format(key))
		};
	}

}
