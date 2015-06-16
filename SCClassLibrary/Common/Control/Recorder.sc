Recorder {

	var <server, <bus, <numChannels;
	var recordBuf, recordNode;

	*new { |server, bus, numChannels|
		^super.newCopyArgs(server, bus, numChannels)
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
				CmdPeriod.doOnce {
					recordNode = nil;
					if (recordBuf.notNil) {
						recordBuf.close {|buf| buf.freeMsg };
						recordBuf = nil
					}
				}
			} {
				recordNode.run(true)
			};
			"Recording: %\n".postf(recordBuf.path);
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
			recordBuf.close({ |buf| buf.freeMsg });
			"Recording Stopped: %\n".postf(recordBuf.path);
			recordBuf = nil;
		} {
			"Not Recording".warn
		}
	}

	prepareForRecord { | path |
		var bufSize = server.recBufSize ?? { server.sampleRate.nextPowerOfTwo };
		var recChannels = numChannels ? server.recChannels;
		var recHeaderFormat = server.recHeaderFormat;
		var recSampleFormat = server.recSampleFormat;
		var bufnum = server.options.numBuffers + 1; // prevent buffer conflicts by using reserved bufnum
		path = path ?? { this.makePath };
		recordBuf = Buffer.alloc(server,
			bufSize,
			recChannels,
			{| buf |
				buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true)
			},
			bufnum
		);
		recordBuf.path = path;

		SynthDef('server-record', { | bufnum |
			DiskOut.ar(bufnum, In.ar(bus ? 0, recChannels))
		}).send(server);
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

		^dir +/+ "SC_" ++ timestamp ++ "." ++ server.recHeaderFormat;
	}


}