TestVolume : UnitTest {

	var server = Server.default;
	var volume = Server.default.volume;
	var level = Server.default.volume.volume;
	var temp;

	test_setVolume {

		this.bootServer;

		this.assert(level == 0, "initial volume is 0 db");
		level = -36;

		server.sync;

		this.assert(volume.ampSynth.notNil, "Server volume synth exists");
		volume.ampSynth.get(\volumeAmp, { |val| temp = val});
		this.assertFloatEquals(temp == level, "volume level correctly set");

		volume.reset;
		server.quit;
		temp = nil;
	}

	test_numOutputs {

		temp = server.options.numOutputBusChannels;

		this.bootServer;

		server.mute;
		server.sync;

		this.assert(server.outputBus.numChannels == volume.numChannels, "volume synth has correct number of channels");

		server.quit;
		server.options.numOutputBusChannels = 8;

		this.bootServer;

		this.assert(server.outputBus.numChannels == volume.numChannels, "volume synth numChannels properly set");

		server.quit;
		server.options.numOutputBusChannels = temp;
		temp = nil;
	}

}
