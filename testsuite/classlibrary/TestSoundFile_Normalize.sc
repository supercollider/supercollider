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
		// tearDown will close 'soundFile'

		file2 = SoundFile.openRead(path2);
		read = Signal.newClear(file2.numFrames);
		file2.readData(read);
		file2.close;
		this.assertEquals([read.first, read.last], [-0.75, 0.75], "Test file normalized to 0.75 should peak at +/-0.75");

		File.delete(path2);
	}
}
