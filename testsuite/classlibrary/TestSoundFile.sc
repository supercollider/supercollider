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
		soundFile = SoundFile(path);

	}

	tearDown {

		soundFile.close;

	}

	test_isOpen {

		soundFile.openRead;
		this.assert(soundFile.isOpen, "SoundFile should now be opened");

		soundFile.close;
		this.assert(soundFile.isOpen.not, "SoundFile should now be closed");

	}

	test_instVars {

		var instVars;

		soundFile.openRead;
		this.assert(soundFile.fileptr.notNil, "SoundFile pointer should not be Nil after opening");

		// collect all instance variables into a Dictionary, remove problematic 'fileptr'
		instVars = soundFile.getSlots.asDict;
		instVars.removeAt(\fileptr);
		
		this.assertEquals(instVars, info, "SoundFile information should match that of actual sound file");

	}

}
