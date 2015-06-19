Recorder {

	var <server;
	var recordBuf, recordNode;
	var <>filePrefix = "SC_";

	*new { |server|
		^super.newCopyArgs(server)
	}

	record { |path|
		if(server.serverRunning.not) {
			"server '%' not running".format(server.name).postln;
			^this
		};
		if(recordBuf.isNil) {
			fork {
				this.prepareForRecord(path);
				server.sync;
				this.record;
			}
		} {
			if(this.isRecording.not) {
				recordNode = Synth.tail(RootNode(server), 'server-record', [\bufnum, recordBuf.bufnum]);
				CmdPeriod.doOnce { this.stopRecording }
			} {
				recordNode.run(true)
			};
			"Recording ... \npath: '%'\n".postf(recordBuf.bpath);
		}
	}

	isRecording {
		^recordNode.isPlaying
	}

	pauseRecording {
		if(recordNode.notNil) {
			recordNode.run(false);
			"Paused".postln
		} {
			"Not Recording".warn
		}
	}

	stopRecording {
		if(recordNode.notNil) {
			recordNode.free;
			recordNode = nil;
			"Recording Stopped.\npath: '%'\n".postf(recordBuf.path);
			if (recordBuf.notNil) {
				recordBuf.close({ |buf| buf.freeMsg });
				recordBuf = nil;
			}
		} {
			"Not Recording".warn
		}
	}

	prepareForRecord { | path, bus, numChannels, recHeaderFormat, recSampleFormat, recBufSize |
		var bufSize = recBufSize ? server.recBufSize ?? { server.sampleRate.nextPowerOfTwo };
		var bufnum = server.options.numBuffers + 1; // prevent buffer conflicts by using reserved bufnum

		recHeaderFormat = recHeaderFormat ? server.recHeaderFormat;
		recSampleFormat = recSampleFormat ? server.recSampleFormat;
		bus = (bus ? 0).asControlInput;
		numChannels = numChannels ? server.recChannels;
		path = path ?? { this.makePath };
		recordBuf = Buffer.alloc(server,
			bufSize,
			numChannels,
			{| buf |
				buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true)
			},
			bufnum
		);
		recordBuf.path = path;

		SynthDef('server-record', { | bufnum |
			DiskOut.ar(bufnum, In.ar(bus ? 0, numChannels))
		}).send(server);
		"Preparing '%' server to record channels %\n".postf(server.name, bus.asControlInput + (0..numChannels - 1));
	}

	makePath {
		var timestamp;
		var dir = thisProcess.platform.recordingsDir;
		if(File.exists(dir).not) {
			thisProcess.platform.recordingsDir.mkdir
		};

		// temporary kludge to fix Date's brokenness on windows
		timestamp = if(thisProcess.platform.name == \windows) {
			Main.elapsedTime.round(0.01)
		} {
			Date.localtime.stamp
		};

		^dir +/+ filePrefix ++ timestamp ++ "." ++ server.recHeaderFormat;
	}


}