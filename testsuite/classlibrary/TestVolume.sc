TestVolume : UnitTest {

	var server = Server.default;

	test_setVolume {

		var serverVolume = server.volume.volume;
		var ampSynthVolume;

		this.bootServer;

		this.assert(serverVolume == 0, "initial volume is 0 db");

		serverVolume = -36;
		server.sync;

		this.assert(server.volume.ampSynth.notNil, "Server volume synth exists");

		server.volume.ampSynth.get(\volumeAmp, { |level| ampSynthVolume = level });
		this.assertFloatEquals(ampSynthVolume == serverVolume, "volume level correctly set");

		server.volume.reset;
		server.quit;
	}

	test_numOutputs {

		var default_numChannels = server.options.numOutputBusChannels;

		server.options.numOutputBusChannels = 8;

		this.bootServer;

		this.assert(server.outputBus.numChannels == server.volume.numChannels, "volume synth has correct number of channels");

		server.quit;
		server.options.numOutputBusChannels = default_numChannels;
	}

}
