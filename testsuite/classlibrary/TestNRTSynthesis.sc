TestNRTSynthesis : UnitTest {
	setUp {}

	tearDown {
		// reset to scsynth
		Server.scsynth;
		Score.program = Server.program;
	}

	// helper method
	performInputTest { |serverType|
		var server, score, condVar = CondVar(), exitCode;
		var inputFile, inputData, outputFile, outputData, sampleRate, sizeDiff;
		var outputFilePath = Platform.defaultTempDir +/+ "nrt_test_%.wav".format(serverType);

		inputFile = SoundFile.openRead(ExampleFiles.child);
		if (inputFile.numChannels != 1) {
			Error("input file is supposed to be a mono file").throw;
		};
		sampleRate = inputFile.sampleRate;
		inputData = FloatArray.newClear(inputFile.numFrames);
		inputFile.readData(inputData);
		if (inputData.size != inputFile.numFrames) {
			Error("could not read input file data").throw;
		};
		// we can already close the file here
		inputFile.close;

		Server.perform(serverType);
		Score.program = Server.program;

		server = Server(\nrt_ ++ serverType,
			options: ServerOptions.new
			.numInputBusChannels_(1)
			.numOutputBusChannels_(1)
		);

		score = Score([
			[0.0, ['/d_recv',
				SynthDef(\pass, {
					Out.ar(0, SoundIn.ar(0));
				}).asBytes
			], Synth.basicNew(\pass, server).newMsg]
		]);


		score.recordNRT(
			outputFilePath: outputFilePath,
			inputFilePath: inputFile.path,
			sampleRate: sampleRate,
			headerFormat: "wav",
			sampleFormat: "int16",
			options: server.options,
			duration: inputFile.duration,
			action: { |e|
				exitCode = e;
				condVar.signalAll;
			}
		);

		if (condVar.waitFor(5, { exitCode.notNil }).not) {
			Error(serverType ++ ": NRT synthesis did not complete").throw;
		};
		this.assertEquals(exitCode, 0, serverType ++ ": NRT synthesis completes without errors");

		outputFile = SoundFile.openRead(outputFilePath);
		if (outputFile.isNil) {
			Error("no output file").throw;
		};
		if (outputFile.numFrames == 0) {
			Error("output file is empty").throw;
		};

		/*
		(serverType ++ ": input file info: frames = %, channels = %, sr = %")
		.format(inputFile.numFrames, inputFile.numChannels, inputFile.sampleRate).postln;

		(serverType ++ ": output file info: frames = %, channels = %, sr = %")
		.format(outputFile.numFrames, outputFile.numChannels, outputFile.sampleRate).postln;
		*/

		this.assertEquals(sampleRate, outputFile.sampleRate,
			serverType ++ ": the output file has the correct sample rate");

		this.assertEquals(1, outputFile.numChannels,
			serverType ++ ": the output file has the correct number of channels");

		sizeDiff = outputFile.numFrames - inputFile.numFrames;

		this.assert(sizeDiff >= 0,
			serverType ++ ": the output file must not be shorter than the input file.");

		this.assert(sizeDiff < server.options.blockSize,
			serverType ++ ": the size difference between input and output file must be smaller than the Server block size.");

		outputData = FloatArray.newClear(outputFile.numFrames);
		outputFile.readData(outputData);
		if (outputData.size != outputFile.numFrames) {
			Error(serverType ++ ": could not read output file data").throw;
		};

		this.assertArrayFloatEquals(inputData, outputData[0..(inputData.size - 1)],
			serverType ++ ": input data should match output data");
		this.assert(outputData[inputData.size..].every(_ == 0),
			serverType ++ ": extra samples in the output data must be all zero");

		// the output file does not need to stay open
		outputFile.close;

		File.delete(outputFilePath);

		server.remove;

		^outputFile;
	}

	test_nrtSynthesisInputWorks {
		var scsynthResult = this.performInputTest(\scsynth);
		var supernovaResult = this.performInputTest(\supernova);
		this.assertEquals(scsynthResult.numFrames, supernovaResult.numFrames,
			"The output file size must be the same for both Servers.");
	}

	// helper method
	performInputChannelMismatchTest { |serverType|
		var server, score, condVar = CondVar(), exitCode;
		var inputFile, inputData, outputFile, outputData;
		var outputFilePath = Platform.defaultTempDir +/+ "nrt_test_%.wav".format(serverType);
		var sampleRate, audioBus;
		var getChannel = { |data, channel, numChannels|
			var result = [];
			forBy(channel, data.size - 1, numChannels) { |i|
				result = result.add(data[i]);
			};
			result;
		};

		inputFile = SoundFile.openRead(ExampleFiles.child);
		if (inputFile.numChannels != 1) {
			Error("input file is supposed to be a mono file").throw;
		};
		sampleRate = inputFile.sampleRate;
		inputData = FloatArray.newClear(inputFile.numFrames);
		inputFile.readData(inputData);
		if (inputData.size != inputFile.numFrames) {
			Error("could not read input file data").throw;
		};
		// we can already close the file here
		inputFile.close;

		Server.perform(serverType);
		Score.program = Server.program;

		server = Server(\nrt_ ++ serverType,
			options: ServerOptions.new
			.numInputBusChannels_(2) // intentionally use a "wrong" input channel count
			.numOutputBusChannels_(4)
		);

		// the bus should start after the last input channel
		audioBus = Bus.audio(server);
		if (audioBus.index != server.options.firstPrivateBus) {
			Error("audio bus should be the first private bus").throw;
		};

		score = Score([
			[0.0, ['/d_recv',
				SynthDef(\mismatch_test, {
					// scale down to keep the numbers between -1 and 1
					var numIn = K2A.ar(NumInputBuses.ir);
					var numOut = K2A.ar(NumOutputBuses.ir);
					// read two input channels. the second channel is supposed to be empty.
					Out.ar(0, SoundIn.ar([0, 1]) ++ numIn ++ numOut);
					// write to audio bus to make sure we're not accidentally overwriting input channels
					Out.ar(audioBus, SinOsc.ar(440));
					// write to input buses to test if inputs are properly zeroed before every control period.
					Out.ar(NumOutputBuses.ir, SinOsc.ar([100, 200]));
				}).asBytes
			], Synth.basicNew(\mismatch_test, server).newMsg]
		]);

		score.recordNRT(
			outputFilePath: outputFilePath,
			inputFilePath: inputFile.path,
			sampleRate: sampleRate,
			headerFormat: "wav",
			sampleFormat: "float", // for lossless serialization and no clipping!
			options: server.options,
			duration: 0.1,
			action: { |e|
				exitCode = e;
				condVar.signalAll;
			}
		);

		if (condVar.waitFor(5, { exitCode.notNil }).not) {
			Error(serverType ++ ": NRT synthesis did not complete").throw;
		};
		this.assertEquals(exitCode, 0, serverType ++ ": NRT synthesis completes without errors");

		outputFile = SoundFile.openRead(outputFilePath);
		if (outputFile.isNil) {
			Error("no output file").throw;
		};
		if (outputFile.numFrames == 0) {
			Error("output file is empty").throw;
		};
		outputData = FloatArray.newClear(outputFile.numFrames * outputFile.numChannels);
		outputFile.readData(outputData);
		if (outputData.size != (outputFile.numFrames * outputFile.numChannels)) {
			Error(serverType ++ ": could not read output file data").throw;
		};

		this.assertEquals(outputFile.numChannels, server.options.numOutputBusChannels,
			serverType ++ ": output file has the correct number of channels");

		// the first channel should match the input file
		this.assertArrayFloatEquals(inputData[0..(outputFile.numFrames-1)], getChannel.(outputData, 0, outputFile.numChannels),
			serverType ++ ": first output channel matches input file");

		// the second channel should be silent
		this.assert(getChannel.(outputData, 1, outputFile.numChannels).every { |f| f == 0 },
			serverType ++ ": second output channel is silent");

		// the third channels contains the number of input bus channels, which should match the Server options and not the input file!
		this.assertEquals(outputData[2], server.options.numInputBusChannels,
			serverType ++ ": NRT synthesis had the correct number of input buses");

		// the fourth channel contains the number fo output bus channels
		this.assertEquals(outputData[3], server.options.numOutputBusChannels,
			serverType ++ ": NRT synthesis had the correct number of output buses");

		outputFile.close;

		File.delete(outputFilePath);

		server.remove;
	}

	test_nrtSynthesisInputChannelMismatch {
		this.performInputChannelMismatchTest(\scsynth);
		this.performInputChannelMismatchTest(\supernova);
	}

	// helper method
	performOutputSizeTest { |serverType|
		var server, score, condVar = CondVar(), exitCode;
		// At 48 kHz, a 1 second sound file should produce exactly 48000 samples.
		// (48000 is a multiple of the Server block size.)
		var sampleRate = 48000, duration = 1.0, numFrames = (sampleRate * duration).asInteger;
		var inputFile, inputFilePath = Platform.defaultTempDir +/+ "nrt_test_in_%.wav".format(serverType);
		var outputFile, outputFilePath = Platform.defaultTempDir +/+ "nrt_test_out_%.wav".format(serverType);

		// create an input file with the appropriate sample rate.
		inputFile = SoundFile.openWrite(inputFilePath, "wav", "int16", 1, sampleRate);
		// the input file is supposed to be longer than the given duration, so we can
		// test that NRT synthesis honors the score duration and not the input file length.
		inputFile.writeData(FloatArray.newClear((duration * 1.5 * sampleRate).asInteger));
		// we can already close the file here
		inputFile.close;
		// verify the file length
		inputFile = SoundFile.openRead(inputFilePath);
		if (inputFile.numFrames < (duration * sampleRate)) {
			Error("input file too short").throw;
		};
		inputFile.close;

		Server.perform(serverType);
		Score.program = Server.program;

		server = Server(\nrt_ ++ serverType,
			options: ServerOptions.new
			.numInputBusChannels_(1)
			.numOutputBusChannels_(2)
		);

		score = Score([
			[0.0, ['/d_recv',
				SynthDef(\test, {
					Out.ar(0, [ SoundIn.ar(0), SinOsc.ar(440) ]);
				}).asBytes
			], Synth.basicNew(\test, server).newMsg]
		]);


		score.recordNRT(
			outputFilePath: outputFilePath,
			inputFilePath: inputFile.path,
			sampleRate: sampleRate,
			headerFormat: "wav",
			sampleFormat: "int16",
			options: server.options,
			duration: duration,
			action: { |e|
				exitCode = e;
				condVar.signalAll;
			}
		);

		if (condVar.waitFor(5, { exitCode.notNil }).not) {
			Error(serverType ++ ": NRT synthesis did not complete").throw;
		};
		this.assertEquals(exitCode, 0, serverType ++ ": NRT synthesis completes without errors");

		outputFile = SoundFile.openRead(outputFilePath);
		if (outputFile.isNil) {
			Error("no output file").throw;
		};
		if (outputFile.numFrames == 0) {
			Error("output file is empty").throw;
		};

		this.assertEquals(outputFile.numFrames, numFrames,
			serverType ++ ": the output file has exactly % samples".format(numFrames));

		outputFile.close;

		File.delete(outputFilePath);

		server.remove;
	}

	test_nrtSynthesisProducesExactOutputSize {
		this.performOutputSizeTest(\scsynth);
		this.performOutputSizeTest(\supernova);
	}
}
