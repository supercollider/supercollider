TestServerOptions : UnitTest {


	test_numAudioChannels {

		var testAudio = { |options|
			var allChannels = options.numPrivateAudioBusChannels
			+ options.reservedNumAudioBusChannels
			+ options.numInputBusChannels
			+ options.numOutputBusChannels;

			this.assert(
				options.numAudioBusChannels == allChannels,
				"sections of audio channels should add up to the total number of audio channels (%)"
				"\n numPrivateAudioBusChannels: %\n numInputBusChannels: %\n numOutputBusChannels: %\n"
				"reservedNumAudioBusChannels: %\n"
				.format(
					options.numAudioBusChannels,
					options.numPrivateAudioBusChannels,
					options.numInputBusChannels,
					options.numOutputBusChannels,
					options.reservedNumAudioBusChannels,
				)
			);

		};

		var testFunc = { |numAudio, numPrivate, numIn, numOut, numReserved|

			var options = ServerOptions.new;
			numAudio !? { options.numAudioBusChannels = numAudio };
			numPrivate !? { options.numPrivateAudioBusChannels = numPrivate };
			numIn !? { options.numInputBusChannels = numIn };
			numOut !? { options.numOutputBusChannels = numOut };
			numReserved !? { options.reservedNumAudioBusChannels = numReserved };
			testAudio.value(options);
		};


		testFunc.(nil, nil, nil, nil);
		testFunc.(32, nil, nil, nil);
		testFunc.(1024, 512, nil, nil);
		testFunc.(nil, nil, 8, 45);
		testFunc.(nil, nil, 120, 120);

		testFunc.(nil, nil, 100, 100);
		testFunc.(nil, nil, 0, 0);
		testFunc.(20, nil, 100, 100);
		testFunc.(20, nil, 100, 100, 20);
		testFunc.(nil, nil, nil, nil, 20);
		testFunc.(nil, nil, 120, 120, 25);

	}


}
