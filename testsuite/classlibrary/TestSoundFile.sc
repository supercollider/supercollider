TestSoundFile : UnitTest {

	var server, soundFile, path, info;

	setUp {

		path = Platform.resourceDir +/+ "sounds/a11wlk01.wav";
		info = IdentityDictionary[
			(\numFrames -> 188893),
			(\numChannels -> 1),
			(\path -> path),
			(\sampleRate -> 44100),
			(\sampleFormat -> "int16"),
			(\headerFormat -> "WAV")
		];

		server = Server(this.class.name);
		this.bootServer(server);
		
		soundFile = SoundFile(path);
		server.sync;
		
	}

	tearDown {

		if (soundFile.isOpen) { soundFile.close };
		server.quit.remove;

	}

	test_isOpen {

		soundFile.openRead;
		server.sync;

		this.assert(soundFile.isOpen, "SoundFile should now be opened");

		soundFile.close;
		server.sync;

		this.assert(soundFile.isOpen.not, "SoundFile should now be closed");

	}

	test_instVars {

		var instVars;

		soundFile.openRead;
		server.sync;

		this.assert(soundFile.fileptr.notNil, "SoundFile pointer should not be Nil after opening");

		// collect all instance variables into a Dictionary, remove problematic 'fileptr'
		instVars = soundFile.getSlots.asDict;
		instVars.removeAt(\fileptr);
		
		this.assertEquals(instVars, info, "SoundFile information should match that of actual sound file");

	}

}
