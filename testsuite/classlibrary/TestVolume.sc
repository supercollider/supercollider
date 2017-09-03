TestVolume : UnitTest {

	test_setVolume {

		var s, ampSynthVolume;
		s = Server.default;

		this.bootServer;
		s.sync;

		this.assert(s.volume.volume == 0, "initial volume is 0 db");

		s.volume.volume = -36; // creates volume synth
		this.assert(s.volume.ampSynth.notNil, "Server volume synth exists");

		s.volume.ampSynth.get(\volumeAmp, { |level| ampSynthVolume = level });
		s.sync;
		this.assertFloatEquals(ampSynthVolume, s.volume.volume.dbamp, "volume level correctly set", 0.0001);

		s.volume.reset;
		s.quit;
	}

	test_numOutputs {

		var s, default_numChannels;
		s = Server.default;
		default_numChannels = s.options.numOutputBusChannels;

		s.options.numOutputBusChannels = 8;

		this.bootServer;
		s.sync;

		this.assert(s.outputBus.numChannels == s.volume.numOutputChannels, "volume synth has correct number of channels");

		s.quit;
		s.options.numOutputBusChannels = default_numChannels;
	}

}
