Recorder {

	var <server, <>numChannels;
	var >recHeaderFormat, >recSampleFormat, >recBufSize;
	var recordBuf, recordNode, synthDef;
	var <paused = false, <duration = 0, <>notifyServer = false;
	var <>filePrefix = "SC_";
	var responder, id;

	*new { |server|
		^super.newCopyArgs(server)
	}

	record { |path, bus, numChannels, node, duration|

		server.ifNotRunning { ^this };
		bus = (bus ? 0).asControlInput;

		if(recordBuf.isNil) {
			fork {
				this.prepareForRecord(path, numChannels);
				server.sync;
				this.record(path, bus, numChannels, node, duration) // now we are ready
			}
		} {
			if(numChannels.notNil and: { numChannels != this.numChannels }) {
				"Cannot change recording number of channels while running".warn;
				^this
			};
			if(path.notNil and: { path.standardizePath != this.path }) {
				"Recording was prepared already with a different path: %\n"
				"Tried with this path: %\n".format(this.path, path.standardizePath).error;
				^this
			};
			if(this.isRecording.not) {
				this.prRecord(bus, node, duration);
				this.changedServer(\recording, true);
				"Recording channels % ... \npath: '%'\n"
				.postf(bus + (0..this.numChannels - 1), recordBuf.path);
			} {
				if(paused) {
					this.resumeRecording
				} {
					"Recording already (% seconds)".format(this.duration).warn
				}
			}
		}
	}

	recordBus { |bus, duration, path, numChannels, node|
		var n = bus.numChannels;
		if(numChannels.notNil and: { n.notNil }) { n = min(numChannels, n) };
		this.record(path, bus.index, n, node, duration)
	}

	isRecording {
		^recordNode.isPlaying
	}

	path {
		^recordBuf !? { recordBuf.path }
	}

	numFrames {
		^recordBuf !? { recordBuf.numFrames }
	}

	pauseRecording {
		if(recordNode.isPlaying) {
			recordNode.run(false);
			this.changedServer(\pausedRecording);
			"... paused recording.\npath: '%'\n".postf(recordBuf.path);
		} {
			"Not Recording".warn
		};
		paused = true;
	}

	resumeRecording {
		if(recordNode.isPlaying) {
			if(paused) {
				recordNode.run(true);
				this.changedServer(\recording, true);
				"Resumed recording ...\npath: '%'\n".postf(recordBuf.path);
			}
		} {
			"Not Recording".warn
		};
		paused = false;
	}

	stopRecording {
		if(synthDef.notNil) {
			this.prStopRecord;
			this.changedServer(\recording, false);
		} {
			"Not Recording".warn
		}
	}

	recHeaderFormat { ^recHeaderFormat ? server.recHeaderFormat }
	recSampleFormat { ^recSampleFormat ? server.recSampleFormat }
	recBufSize { ^recBufSize ?? { server.recBufSize } ?? { server.sampleRate.nextPowerOfTwo } }

	prepareForRecord { | path, numChannels |
		var dir;

		numChannels = numChannels ? server.recChannels;

		path = if(path.isNil) { this.makePath } { path.standardizePath };
		dir = path.dirname;
		if(File.exists(dir).not) { dir.mkdir };

		recordBuf = Buffer.alloc(server,
			this.recBufSize,
			numChannels,
			{| buf |
				buf.writeMsg(path, this.recHeaderFormat, this.recSampleFormat, 0, 0, true)
			}
		);
		if(recordBuf.isNil) { Error("could not allocate buffer").throw };
		recordBuf.path = path;
		this.numChannels = numChannels;
		id = UniqueID.next;

		synthDef = SynthDef(SystemSynthDefs.generateTempName, { |in, bufnum, duration|
			var tick = Impulse.kr(1);
			var timer = PulseCount.kr(tick) - 1;
			var doneAction = if(duration <= 0, 0, 2);
			Line.kr(0, 0, duration, doneAction:doneAction);
			SendReply.kr(tick, '/recordingDuration', timer, id);
			DiskOut.ar(bufnum, In.ar(in, numChannels))
		}).send(server);

		"Preparing recording on '%'\n".postf(server.name);
	}

	/* private implementation */

	prRecord { |bus, node, dur|
		recordNode = Synth.tail(node ? 0, synthDef.name, [\bufnum, recordBuf, \in, bus, \duration, dur ? -1]);
		recordNode.register(true);
		recordNode.onFree { this.stopRecording };
		if(responder.isNil) {
			responder = OSCFunc({ |msg|
				if(msg[2] == id) {
					duration = msg[3];
					this.changedServer(\recordingDuration, duration);
				}
			}, '/recordingDuration', server.addr);
		} {
			responder.enable;
		};
	}

	prStopRecord {
		var recordPath;
		if(recordNode.isPlaying) { recordNode.unregister; recordNode.free; recordNode = nil };
		server.sendMsg("/d_free", synthDef.name);
		synthDef = nil;
		if(recordBuf.notNil) {
			recordPath = recordBuf.path;
			recordBuf.close({ |buf| buf.freeMsg });
			"Recording Stopped: (%)\n".postf(recordPath.basename);
			recordBuf = nil
		};
		responder.disable;
		paused = false;
		duration = 0;
		this.changedServer(\recordingDuration, 0);
	}

	makePath {
		var timestamp;
		var dir = thisProcess.platform.recordingsDir;
		timestamp = Date.localtime.stamp;
		^dir +/+ filePrefix ++ timestamp ++ "." ++ this.recHeaderFormat;
	}

	changedServer { | what ... moreArgs |
		if(notifyServer) { server.changed(what, *moreArgs) }
	}


}
