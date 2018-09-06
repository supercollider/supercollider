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

	// tests several features:
	// openRead and openWrite class methods (not tested above)
	// readData and writeData
	test_readWrite {
		var path = PathName.tmp +/+ "SoundFile-test%.aiff".format(this.hash.asHexString(8)),
		file, written = Signal.fill(100, { 1.0.rand2 }), read;

		file = SoundFile.openWrite(path, "aiff", "float", 1, 44100);
		if(file.isNil) {
			this.assert(false, "SoundFile.openWrite into temp directory should not fail");
		};
		try {
			file.writeData(written);
		} { |error|
			this.assert(false, "SoundFile:writeData should not fail with error " ++ error.errorString);
		};
		file.close;

		file = SoundFile.openRead(path);
		this.assertEquals(file.numFrames, written.size, "SoundFile size should match data size written by writeData");

		try {
			read = Signal.newClear(file.numFrames);
			file.readData(read);
		} { |error|
			this.assert(false, "SoundFile:readData should not fail with error " ++ error.errorString);
		};
		file.close;

		this.assertEquals(written, read, "SoundFile:readData should get the same data that were written");

		File.delete(path);
	}

}

TestSoundFile_Normalize : UnitTest {
	var soundFile, path, written;

	setUp {
		path = PathName.tmp +/+ "testSFNorm%.aiff".format(this.hash.asHexString(8));
		soundFile = SoundFile(path);
		written = Signal.series(100, -0.5, 1/99);
		soundFile.openWrite;
		soundFile.writeData(written);
		soundFile.close;
	}

	tearDown {
		soundFile.close;
		File.delete(path);
	}

	test_channelPeaks {
		var peaks;
		soundFile.openRead;
		if(soundFile.isOpen) {
			peaks = soundFile.channelPeaks;
			this.assertEquals(peaks, [0.5], "Channel peaks test should read 0.5");
		} {
			this.assert(false, "TestSoundFile_Normalize should be able to read the test file");
		};
		soundFile.close;
	}

	test_normalize {
		var path2 = path.splitext[0] ++ "-normalized.aiff",
		file2, read;

		soundFile.openRead;
		// note: 0.75 because the denominator of 3/4 is a power of two
		// so you will not have floating point rounding error in the equality test below
		// if you change this value, please make sure the denominator is a power of two!
		soundFile.normalize(path2, "aiff", "float", maxAmp: 0.75);
		soundFile.close;

		file2 = SoundFile.openRead(path2);
		read = Signal.newClear(file2.numFrames);
		file2.readData(read);
		file2.close;
		this.assertEquals([read.first, read.last], [-0.75, 0.75], "Test file normalized to 0.75 should peak at +/-0.75");

		File.delete(path2);
	}
}
