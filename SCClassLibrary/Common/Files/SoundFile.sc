/*
	Sound File Format strings:
		header formats:
			read/write formats:
				"AIFF", 	- Apple's AIFF
				"WAV","RIFF" 	- Microsoft .WAV
				"SD2", 	- Sound Designer 2
				"Sun", 	- NeXT/Sun
				"IRCAM", 	- old IRCAM format
				"none"	- no header = raw data
			A huge number of other formats are supported read only.

		sample formats:
			"int8", "int16", "int24", "int32"
			"mulaw", "alaw",
			"float"
		not all header formats support all sample formats.
*/

SoundFile {
	classvar <openFiles;

	var <>fileptr;
	var <>headerFormat = "AIFF";
	var <>sampleFormat = "float";
	var <numFrames = 0;		// number of frames
	var <>numChannels = 1;	// number of channels
	var <>sampleRate = 44100.0;
	var <> path;


	*closeAll {
		if (openFiles.notNil, {
			openFiles.copy.do({ arg file; file.close; });
		});
	}
	isOpen {
		^fileptr.notNil
	}

	*new { arg pathName;
		^super.new.path_(pathName);
	}

	*openRead{ arg pathName;
		var file;
		file = SoundFile(pathName);
		if(file.openRead(pathName)){^file}{^nil}
	}

	*openWrite{ arg pathName;
		var file;
		file = SoundFile(pathName);
		if(file.openWrite(pathName)){ ^file}{^nil}
	}

	*use { arg path, function;
		var file = this.new, res;
		protect {
			file.openRead(path);
			res = function.value(file);
		} {
			file.close;
		}
		^res
	}

	openRead{ arg pathName;
		path = pathName ? path;
		^this.prOpenRead(path);
	}

	prOpenRead { arg pathName;
		// returns true if success, false if file not found or error reading.
		_SFOpenRead
		^this.primitiveFailed;
	}

	readData { arg rawArray;
		// must have called openRead first!
		// returns true if success, false if file not found or error reading.
		_SFRead
		^this.primitiveFailed;
	}

	readHeaderAsString {
		// must have called openRead first!
		//returns the whole header as String
		_SFHeaderInfoString
		^this.primitiveFailed;

	}

	openWrite{ arg pathName;
		pathName = pathName ? path;
		^this.prOpenWrite(pathName)
	}

	prOpenWrite { arg pathName;
		// write the header
		// format written is that indicated in headerFormat and sampleFormat.
		// return true if successful, false if not found or error writing.
		_SFOpenWrite
		^this.primitiveFailed;
	}
	writeData { arg rawArray;
		// must have called openWrite first!
		// format written is that indicated in sampleFormat.
		// return true if successful, false if not found or error writing.
		_SFWrite
		^this.primitiveFailed;
	}
	close {
		_SFClose
		^this.primitiveFailed;
	}

	seek { arg offset = 0, origin = 0;
		// offset is in frames
		// origin is an integer, one of:
		// 0 - from beginning of file
		// 1 - from current position
		// 2 - from end of file
		_SFSeek
		^this.primitiveFailed;
	}

	duration { ^numFrames/sampleRate }



		// normalizer utility

	*normalize { |path, outPath, newHeaderFormat, newSampleFormat,
		startFrame = 0, numFrames, maxAmp = 1.0, linkChannels = true, chunkSize = 4194304,
		threaded = false|

		var	file, outFile,
			action = {
				protect {
					outFile = file.normalize(outPath, newHeaderFormat, newSampleFormat,
						startFrame, numFrames, maxAmp, linkChannels, chunkSize, threaded);
				} { file.close };
				file.close;
			};

		(file = SoundFile.openRead(path.standardizePath)).notNil.if({
				// need to clean up in case of error
			if(threaded, {
				Routine(action).play(AppClock)
			}, action);
			^outFile
		}, {
			MethodError("Unable to read soundfile at: " ++ path, this).throw;
		});
	}

	normalize { |outPath, newHeaderFormat, newSampleFormat,
		startFrame = 0, numFrames, maxAmp = 1.0, linkChannels = true, chunkSize = 4194304,
		threaded = false|

		var	peak, outFile;

		outFile = SoundFile.new.headerFormat_(newHeaderFormat ?? { this.headerFormat })
			.sampleFormat_(newSampleFormat ?? { this.sampleFormat })
			.numChannels_(this.numChannels)
			.sampleRate_(this.sampleRate);

			// can we open soundfile for writing?
		outFile.openWrite(outPath.standardizePath).if({
			protect {
				"Calculating maximum levels...".postln;
				peak = this.channelPeaks(startFrame, numFrames, chunkSize, threaded);
				Post << "Peak values per channel are: " << peak << "\n";
				peak.includes(0.0).if({
					MethodError("At least one of the soundfile channels is zero. Aborting.",
						this).throw;
				});
					// if all channels should be scaled by the same amount,
					// choose the highest peak among all channels
					// otherwise, retain the array of peaks
				linkChannels.if({ peak = peak.maxItem });
				"Writing normalized file...".postln;
				this.scaleAndWrite(outFile, maxAmp / peak, startFrame, numFrames, chunkSize,
					threaded);
				"Done.".postln;
			} { outFile.close };
			outFile.close;
			^outFile
		}, {
			MethodError("Unable to write soundfile at: " ++ outPath, this).throw;
		});
	}

	*groupNormalize { |paths, outDir, newHeaderFormat, newSampleFormat,
		maxAmp = 1.0, chunkSize = 4194304,
		threaded = true|

		var action;

		action = {
			var groupPeak = 0.0, files, outFiles;

			"Calculating maximum levels...".postln;
			paths.do({|path|
				var	file, peak;

				(file = SoundFile.openRead(path.standardizePath)).notNil.if({
					"Checking levels for file %\n".postf(path.standardizePath);
					files = files.add(file);

						peak = file.channelPeaks(0, nil, chunkSize, threaded);
						Post << "Peak values per channel are: " << peak << "\n";
						peak.includes(0.0).if({
							MethodError("At least one of the soundfile channels is zero. Aborting.",
								this).throw;
						});

					groupPeak = max(groupPeak, peak.maxItem);

				}, {
					MethodError("Unable to read soundfile at: " ++ path, this).throw;
				});
			});

			"Overall peak level: %\n".postf(groupPeak);
			outDir = outDir.standardizePath.withTrailingSlash;

			files.do({|file|

				var outPath, outFile;

				outPath = outDir ++ file.path.basename;

				outFile = SoundFile.new.headerFormat_(newHeaderFormat ?? { file.headerFormat })
					.sampleFormat_(newSampleFormat ?? { file.sampleFormat })
					.numChannels_(file.numChannels)
					.sampleRate_(file.sampleRate);

				outFile.openWrite(outPath).if({
					protect {
						"Writing normalized file %\n".postf(outPath);
						file.scaleAndWrite(outFile, maxAmp / groupPeak, 0, nil, chunkSize,
							threaded);
					} { outFile.close };
					outFile.close;
				}, {
					MethodError("Unable to write soundfile at: " ++ outPath, this).throw;
				});

			});

			"////// Group Normalize complete //////".postln;

		};

		if(threaded, {
			Routine(action).play(AppClock)
		}, action);

	}

	channelPeaks { |startFrame = 0, numFrames, chunkSize = 1048576, threaded = false|
		var rawData, peak, numChunks, chunksDone, test;

		peak = 0 ! numChannels;
		numFrames.isNil.if({ numFrames = this.numFrames });
		numFrames = numFrames * numChannels;

			// chunkSize must be a multiple of numChannels
		chunkSize = (chunkSize/numChannels).floor * numChannels;

		if(threaded) {
			numChunks = (numFrames / chunkSize).roundUp(1);
			chunksDone = 0;
		};

		this.seek(startFrame, 0);

		{	(numFrames > 0) and: {
				rawData = FloatArray.newClear(min(numFrames, chunkSize));
				this.readData(rawData);
				rawData.size > 0
			}
		}.while({
			rawData.do({ |samp, i|
				(samp.abs > peak[i % numChannels]).if({
					peak[i % numChannels] = samp.abs
				});
			});
			numFrames = numFrames - chunkSize;
			if(threaded) {
				chunksDone = chunksDone + 1;
				test = chunksDone / numChunks;
				(((chunksDone-1) / numChunks) < test.round(0.02) and: { test >= test.round(0.02) }).if({
					$..post;
				});
				0.0001.wait;
			};
		});
		if(threaded) { $\n.postln };
		^peak
	}

	scaleAndWrite { |outFile, scale, startFrame, numFrames, chunkSize, threaded = false|
		var	rawData, numChunks, chunksDone, test;

		numFrames.isNil.if({ numFrames = this.numFrames });
		numFrames = numFrames * numChannels;
		scale = scale.asArray;
//		(scale.size == 0).if({ scale = [scale] });

			// chunkSize must be a multiple of numChannels
		chunkSize = (chunkSize/numChannels).floor * numChannels;

		if(threaded) {
			numChunks = (numFrames / chunkSize).roundUp(1);
			chunksDone = 0;
		};

		this.seek(startFrame, 0);

		{	(numFrames > 0) and: {
				rawData = FloatArray.newClear(min(numFrames, chunkSize));
				this.readData(rawData);
				rawData.size > 0
			}
		}.while({
			rawData.do({ |samp, i|
				rawData[i] = rawData[i] * scale.wrapAt(i)
			});
				// write, and check whether successful
				// throwing the error invokes error handling that closes the files
			(outFile.writeData(rawData) == false).if({
				MethodError("SoundFile writeData failed.", this).throw
			});

			numFrames = numFrames - chunkSize;
			if(threaded) {
				chunksDone = chunksDone + 1;
				test = chunksDone / numChunks;
				(((chunksDone-1) / numChunks) < test.round(0.02) and: { test >= test.round(0.02) }).if({
					$..post;
				});
				0.0001.wait;
			};
		});
		if(threaded) { $\n.postln };
		^outFile
	}

		// diskIn synthdefs are now created on demand in SoundFile:cue
//	*initClass {
//		StartUp.add {
//			(1..16).do { | i |
//				SynthDef("diskIn" ++ i, { | out, amp = 1, bufnum, sustain, ar = 0, dr = 0.01 gate = 1 |
//					Out.ar(out, DiskIn.ar(i, bufnum)
//					* Linen.kr(gate, ar, 1, dr, 2)
//					* EnvGen.kr(Env.linen(ar, sustain - ar - dr max: 0 ,dr),1, doneAction: 2) * amp)
//				}).store
//			}
//		};
//	}

	info { | path |
		var flag = this.openRead;
		if (flag) {
			this.close;
		} {
			^nil
		}
	}

	*collect { | path = "sounds/*" |
		var paths, files;
		paths = path.pathMatch;
		files = paths.collect { | p | SoundFile(p).info };
		files = files.select(_.notNil);
		^files;
	}

	*collectIntoBuffers { | path = "sounds/*", server |
		server = server ?? { Server.default };
		if (server.serverRunning) {
			^SoundFile.collect(path)
				.collect { |  sf |
					Buffer(server, sf.numFrames, sf.numChannels)
					.allocRead(sf.path)
					.sampleRate_(sf.sampleRate);
				}
		} {
			"the server must be running to collect soundfiles into buffers".error
		}
	}


	asBuffer { |server|
		var buffer, rawData;
		server = server ? Server.default;
		if(server.serverRunning.not) { Error("SoundFile:asBuffer - Server not running.").throw };
		if(this.isOpen.not) { Error("SoundFile:asBuffer - SoundFile not open.").throw };
		if(server.isLocal) {
			buffer = Buffer.read(server, path)
		} {
			forkIfNeeded {
				buffer = Buffer.alloc(server, numFrames, numChannels);
				rawData = FloatArray.newClear(numFrames * numChannels);
				this.readData(rawData);
				server.sync;
				buffer.sendCollection(rawData, wait: -1);
			}
		};
		^buffer
	}

	cue { | ev, playNow = false, closeWhenDone = false |
		var server, packet, defname = "diskIn" ++ numChannels, condition, onClose;
		ev = ev ? ();
		if (this.numFrames == 0) { this.info };
		fork {
			ev.use {
				server = ~server ?? { Server.default};
				if(~instrument.isNil) {
					SynthDef(defname, { | out, amp = 1, bufnum, sustain, ar = 0, dr = 0.01 gate = 1 |
						Out.ar(out, VDiskIn.ar(numChannels, bufnum, BufRateScale.kr(bufnum) )
						* Linen.kr(gate, ar, 1, dr, 2)
						* EnvGen.kr(Env.linen(ar, sustain - ar - dr max: 0 ,dr),1, doneAction: 2) * amp)
					}).add;
					~instrument = defname;
					condition = Condition.new;
					server.sync(condition);
				};
				ev.synth;	// set up as a synth event (see Event)
				~bufnum =  server.bufferAllocator.alloc(1);
				~bufferSize = 0x10000;
				~firstFrame = ~firstFrame ? 0;
				~lastFrame = ~lastFrame ? numFrames;
				~sustain = (~lastFrame - ~firstFrame)/(sampleRate ?? {server.options.sampleRate ? 44100});
				~close = { | ev |
						server.bufferAllocator.free(ev[\bufnum]);
						server.sendBundle(server.latency, ["/b_close", ev[\bufnum]],
							["/b_free", ev[\bufnum] ]  )
				};
				~setwatchers = { |ev|
					OSCFunc({
						server.sendBundle(server.latency, ["/b_close", ev[\bufnum]],
							["/b_read", ev[\bufnum], path, ev[\firstFrame], ev[\bufferSize], 0, 1]);
					}, "/n_end", server.addr, nil, ev[\id][0]).oneShot;
				};
				if (playNow) {
					packet = server.makeBundle(false, {ev.play})[0];
						// makeBundle creates an array of messages
						// need one message, take the first
				} {
					packet = [];
				};
				server.sendBundle(server.latency,["/b_alloc", ~bufnum, ~bufferSize, numChannels,
							["/b_read", ~bufnum, path, ~firstFrame, ~bufferSize, 0, 1, packet]
						]);
			};
		};
		if (closeWhenDone) {
			onClose = SimpleController(ev).put(\n_end, {
				ev.close;
				onClose.remove;
			});
			ev.addDependant(onClose)
		};
		^ev;
	}

	play { | ev, playNow = true |
		^this.cue(ev, playNow)
	}

	asEvent { | type = \allocRead |
		if (type == \cue) {
			^(	type: 			type,
				path: 			path,
				numFrames: 		numFrames,
				sampleRate: 		sampleRate,
				numChannels: 		numChannels,
				bufferSize:		0x10000,
				firstFileFrame:	0,
				firstBufferFrame: 	0,
				leaveOpen:		1
			)
		} {
			^(	type: 			type,
				path: 			path,
				numFrames: 		numFrames,
				sampleRate: 		sampleRate,
				numChannels: 		numChannels,
				firstFileFrame:	0
			)
		}

	}

	toCSV { |outpath, headers, delim=",", append=false, func, action|

		var outfile, dataChunk;

		// Prepare input
		if(this.openRead(this.path).not){
			^"SoundFile:toCSV could not open the sound file".error
		};
		dataChunk = FloatArray.newClear(this.numChannels * min(this.numFrames, 1024));

		// Prepare output
		if(outpath.isNil){	outpath = path.splitext.at(0) ++ ".csv" };
		outfile = File(outpath, if(append, "a", "w"));
		if(headers.notNil){
			if(headers.isString){
				outfile.write(headers ++ Char.nl);
			}{
				outfile.write(headers.join(delim) ++ Char.nl);
			}
		};

		// Now do it
		while{this.readData(dataChunk); dataChunk.size > 0}{
			dataChunk.clump(this.numChannels).do{|row|
				outfile.write(if(func.isNil, {row}, {func.value(row)}).join(delim) ++ Char.nl)
			};
		};
		outfile.close;
		this.close;

		action.value(outpath);
	}

}
