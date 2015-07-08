Recorder {

	var <server, <>numChannels;
	var <>recHeaderFormat, <>recSampleFormat, <>recBufSize;
	var recordBuf, recordNode, synthDef;
	var <>paused = false;
	var <>filePrefix = "SC_";

	*new { |server|
		^super.newCopyArgs(server)
	}

	record { |path, bus, numChannels, node|

		server.ifNotRunning { ^this };

		if(recordBuf.isNil) {
			fork {
				this.prepareForRecord(path, numChannels);
				server.sync;
				this.record(path, bus, numChannels, node)
			}
		} {
			if(numChannels.notNil and: { numChannels != this.numChannels }) {
				"Cannot change recording number of channels while running".warn;
				^this
			};
			if(this.isRecording.not) {
				bus = (bus ? 0).asControlInput;
				recordNode = Synth.tail(node ? 0, synthDef.name, [\bufnum, recordBuf, \in, bus]);
				recordNode.register(true);
				CmdPeriod.add(this);
				numChannels = this.numChannels;
				"Recording channels % ... \npath: '%'\n"
				.postf(bus + (0..numChannels - 1), recordBuf.path);
			} {
				if(paused) { this.resumeRecording };
			};
			server.changed(\recording, true);

		}
	}

	cmdPeriod {
		this.stopRecording
	}

	isRecording {
		^recordNode.isPlaying
	}

	pauseRecording {
		if(recordNode.notNil) {
			recordNode.run(false);
			server.changed(\pausedRecording);
			"... paused recording.\npath: '%'\n".postf(recordBuf.path);
		};
		paused = true;
	}

	resumeRecording {
		if(recordNode.isPlaying) {
			if(paused) {
				recordNode.run(true);
				server.changed(\recording, true);
				"Resumed recording ...\npath: '%'\n".postf(recordBuf.path);
			}
		} {
			"Not Recording".warn
		};
		paused = false;
	}

	stopRecording {
		if(recordNode.notNil) {
			recordNode.free;
			recordNode = nil;
			server.sendMsg("/d_free", synthDef.name);
			synthDef = nil;
			server.changed(\recording, false);
			"Recording stopped, written to\npath: '%'\n".postf(recordBuf.path);
			if (recordBuf.notNil) {
				recordBuf.close({ |buf| buf.freeMsg });
				recordBuf = nil;
			};
			CmdPeriod.remove(this);
		} {
			"Not Recording".warn
		};
		paused = false;
	}

	prepareForRecord { | path, numChannels |
		var bufSize = recBufSize ? server.recBufSize ?? { server.sampleRate.nextPowerOfTwo };

		recHeaderFormat = recHeaderFormat ? server.recHeaderFormat;
		recSampleFormat = recSampleFormat ? server.recSampleFormat;
		numChannels = numChannels ? server.recChannels;
		path = if(path.isNil) { this.makePath } { path.standardizePath };
		recordBuf = Buffer.alloc(server,
			bufSize,
			numChannels,
			{| buf |
				buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true)
			}
		);
		recordBuf.path = path;
		this.numChannels = numChannels;

		synthDef = SynthDef(SystemSynthDefs.generateTempName, { |in, bufnum|
			DiskOut.ar(bufnum, In.ar(in, numChannels))
		}).send(server);

		"Preparing recording on '%'\n".postf(server.name);
	}

	makePath {
		var timestamp;
		var dir = thisProcess.platform.recordingsDir;
		if(File.exists(dir).not) {
			dir.mkdir;
			"created recordings directory: '%'\n".postf(dir)
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