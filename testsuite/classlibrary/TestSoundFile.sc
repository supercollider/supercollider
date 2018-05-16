TestSoundFile : UnitTest {

	var soundFile, path;

	setUp {

		path = Platform.resourceDir +/+ "sounds/a11wlk01.wav";
		soundFile = SoundFile(path);

	}

	tearDown {

		soundFile.close;

	}

	test_isOpen {

		soundFile.openRead;
		this.assert(soundFile.isOpen, "SoundFile should be opened");

	}

	test_close {

		soundFile.openRead;
		soundFile.close;
		this.assert(soundFile.isOpen.not, "SoundFile should be closed");

	}

	test_instVars {

		var instVars;
		var info = IdentityDictionary[
			(\numFrames -> 188893),
			(\numChannels -> 1),
			(\path -> path),
			(\sampleRate -> 44100),
			(\sampleFormat -> "int16"),
			(\headerFormat -> "WAV")
		];

		soundFile.openRead;
		this.assert(soundFile.fileptr.notNil, "SoundFile pointer should not be Nil after opening");

		// collect all instance variables into a Dictionary, remove problematic 'fileptr'
		instVars = soundFile.getSlots.asDict;
		instVars.removeAt(\fileptr);
		
		this.assertEquals(instVars, info, "SoundFile information should match that of actual sound file");

	}

	test_cue {

		var cueEvent, testEvent, group;
		var server = Server(this.class.name);
		this.bootServer(server);

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
