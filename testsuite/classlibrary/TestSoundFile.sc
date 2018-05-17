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

}
