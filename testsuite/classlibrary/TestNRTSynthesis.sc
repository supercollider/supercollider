TestNRTSynthesis : UnitTest {
	setUp {}

	tearDown {
		// reset to scsynth
		Server.scsynth;
		Score.program = Server.program;
	}

	// helper method
	performInputTest { |serverType|
		var server, score, condVar = CondVar(), done = false;
		var inputFile, sampleRate, inputData, outputFile, outputData, sizeDiff;
		var outputFilePath = Platform.defaultTempDir +/+ "nrt_test_%.wav".format(serverType);

		inputFile = SoundFile.openRead(ExampleFiles.child);
		sampleRate = inputFile.sampleRate;
		inputData = FloatArray.newClear(inputFile.numFrames);
		inputFile.readData(inputData);
		if (inputData.size != (inputFile.numFrames * inputFile.numChannels)) {
			Error("could not read input file data").throw;
		};
		// we can already close the file here
		inputFile.close;

		// Server.supernova;
		Server.perform(serverType);
		Score.program = Server.program;

		server = Server(\nrt_ ++ serverType,
			options: ServerOptions.new
			.numOutputBusChannels_(1)
			.numInputBusChannels_(1)
		);

		score = Score([
			[0.0, ['/d_recv',
				SynthDef(\pass, {
					var in = SoundIn.ar(0);
					Out.ar(0, in);
				}).asBytes
			]],
			[0.0, Synth.basicNew(\pass, server).newMsg]
		]);


		score.recordNRT(
			outputFilePath: outputFilePath,
			inputFilePath: inputFile.path,
			sampleRate: sampleRate,
			headerFormat: "wav",
			sampleFormat: "int16",
			options: server.options,
			duration: inputFile.duration,
			action: { done = true; condVar.signalAll }
		);

		if (condVar.waitFor(5, { done }).not) {
			Error(serverType ++ ": NRT synthesis did not complete").throw;
		};

		outputFile = SoundFile.openRead(outputFilePath);
		if (outputFile.numFrames == 0) {
			Error("output file is empty").throw;
		};

		(serverType ++ ": input file info: frames = %, channels = %, sr = %")
		.format(inputFile.numFrames, inputFile.numChannels, inputFile.sampleRate).postln;

		(serverType ++ ": output file info: frames = %, channels = %, sr = %")
		.format(outputFile.numFrames, outputFile.numChannels, outputFile.sampleRate).postln;

		this.assertEquals(sampleRate, outputFile.sampleRate,
			serverType ++ ": the output file has the correct sample rate");

		this.assertEquals(server.options.numOutputBusChannels, outputFile.numChannels,
			serverType ++ ": the output file has the correct number of channels");

		sizeDiff = outputFile.numFrames - inputFile.numFrames;

		this.assert(sizeDiff >= 0,
			serverType ++ ": the output file must not be shorter than the input file.");

		this.assert(sizeDiff < server.options.blockSize,
			serverType ++ ": the size difference between input and output file must be smaller than the Server block size.");

		outputData = FloatArray.newClear(outputFile.numFrames);
		outputFile.readData(outputData);
		if (outputData.size != (outputFile.numFrames * outputFile.numChannels)) {
			Error(serverType ++ ": could not read output file data").throw;
		};
		// the output file does not need to stay open
		outputFile.close;

		server.remove;

		this.assertArrayFloatEquals(inputData, outputData[0..(inputData.size - 1)],
			serverType ++ ": input data should match output data");
		this.assert(outputData[inputData.size..].every(_ == 0),
			serverType ++ ": extra samples in the output data must be all zero");

		^outputFile;
	}

	test_nrtSynthesisInputWorks {
		var scsynthResult = this.performInputTest(\scsynth);
		var supernovaResult = this.performInputTest(\supernova);
		this.assertEquals(scsynthResult.numFrames, supernovaResult.numFrames,
			"The output file size must be the same for both Servers.");
	}
}
