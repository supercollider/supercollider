Recorder {

	var <server, <>numChannels;
	var <>recHeaderFormat, <>recSampleFormat, <>recBufSize;
	var recordBuf, recordNode, synthDef;
	var <paused = false, <duration = 0;
	var <>filePrefix = "SC_";
	var responder, id;

	*new { |server|
		^super.newCopyArgs(server)
	}

	record { |path, bus, numChannels, node, duration|

		server.ifNotRunning { ^this };
		if(recordBuf.isNil) {
			fork {
				this.prepareForRecord(path, numChannels);
				server.sync;
				this.record(path, bus, numChannels, node, duration)
			}
		} {
			if(numChannels.notNil and: { numChannels != this.numChannels }) {
				"Cannot change recording number of channels while running".warn;
				^this
			};
			if(this.isRecording.not) {
				this.prStartListen;
				bus = (bus ? 0).asControlInput;
				recordNode = Synth.tail(node ? 0, synthDef.name, [\bufnum, recordBuf, \in, bus, \duration, duration ? -1]);
				recordNode.register(true);
				recordNode.onFree { this.stopRecording };
				CmdPeriod.add(this);

				server.changed(\recording, true);
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


	cmdPeriod {
		this.stopRecording
	}

	isRecording {
		^recordNode.isPlaying
	}

	pauseRecording {
		if(recordNode.isPlaying) {
			recordNode.run(false);
			server.changed(\pausedRecording);
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
				server.changed(\recording, true);
				"Resumed recording ...\npath: '%'\n".postf(recordBuf.path);
			}
		} {
			"Not Recording".warn
		};
		paused = false;
	}

	stopRecording {
		if(synthDef.notNil) {
			this.freeResources;
			server.changed(\recording, false);
		} {
			"Not Recording".warn
		};
		paused = false;
		duration = 0;
		server.changed(\recordingDuration, 0);
		this.prStopListen;
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
		id = UniqueID.next;

		synthDef = SynthDef(SystemSynthDefs.generateTempName, { |in, bufnum, duration|
			var tick = Impulse.kr(1);
			var timer = PulseCount.kr(tick);
			var doneAction = if(duration <= 0, 0, 2);
			Line.kr(0, 0, duration, doneAction:doneAction);
			SendReply.kr(tick, '/recordingDuration', timer, id);
			DiskOut.ar(bufnum, In.ar(in, numChannels))
		}).send(server);

		"Preparing recording on '%'\n".postf(server.name);
	}

	freeResources {
		if(recordNode.isPlaying) { recordNode.unregister; recordNode.free; recordNode = nil };
		server.sendMsg("/d_free", synthDef.name);
		synthDef = nil;
		if(recordBuf.notNil) { recordBuf.close({ |buf| buf.freeMsg }); recordBuf = nil };
		CmdPeriod.remove(this);
	}


	prStartListen {
		if(responder.isNil) {
			responder = OSCFunc({ |msg|
				if(msg[2] == id) {
					duration = msg[3];
					server.changed(\recordingDuration, duration);
				}
			}, '/recordingDuration', server.addr);
		} {
			responder.enable;
		}
	}

	prStopListen {
		responder.disable;
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