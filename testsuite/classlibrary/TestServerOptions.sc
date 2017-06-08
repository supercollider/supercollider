TestServerOptions : UnitTest {


	test_numAudioChannels {
		var options = ServerOptions.new;

		var testAudio = {
			this.assert(
				options.numAudioBusChannels == (
					options.numPrivateAudioBusChannels + options.numInputBusChannels + options.numOutputBusChannels
				),
				"sections of audio channels should add up to the total number of audio channels\nnumPrivateAudioBusChannels: %, numInputBusChannels: %, numOutputBusChannels: %\n"
				.format(options.numPrivateAudioBusChannels, options.numInputBusChannels, options.numOutputBusChannels)
			);

		};

		testAudio.value;
		options.numAudioBusChannels = rrand(16, 1000);
		testAudio.value;
		options.numPrivateAudioBusChannels = rrand(16, 1000);
		testAudio.value;
		options.numInputBusChannels = rrand(4, 8);
		testAudio.value;
		options.numOutputBusChannels = rrand(4, 8);

	}


}
