
BufEnvir : EnvironmentRedirect {
	var <server;
	var <>fftsize = 2048;

	*new { arg server;
		^super.new.initServer(server)
	}

	initServer { arg s; server = s }

	makeBuffer { ^Buffer.new(server, 0, 1) }
	updateInfo { envir.do(_.updateInfo) }

	clear { arg removeReferences = false;
		this.do { |buf| buf.free };
		if(removeReferences) { super.clear };
	}

	put { arg key, obj;
		var buf, bufnum;
		if(server.serverRunning.not) { Error("server not running").throw };
		if(key.isNil) { Error("BufEnvir:put() key is nil").throw };
		if(key.isSequenceableCollection) { ^this.putAll(key, obj.asArray) };
		server.makeBundle(nil, {
			buf = this.at(key);
			bufnum = buf.bufnum;
			obj.toBuffer(buf);
			buf.updateInfo;
		});
		super.put(key, buf)
	}

	at { arg key;
		var res;
		if(key.isNil) { Error("BufEnvir:at() key is nil").throw };
		if(key.isSequenceableCollection) { ^this.getAll(key) };
		res = envir.at(key);
		if(server.serverRunning.not) { Error("server not running").throw };
		if(res.isNil) {
			res = this.makeBuffer;
			envir.put(key, res);
		};
		^res
	}

	putAll { arg keys, objects;
		keys.do { |key, i|
			this.put(key, objects.wrapAt(i))
		}
	}
	getAll { arg keys;
		^keys.collect { |key|
			this.at(key)
		}
	}
	putSeries { arg first, second, last, value;
		this.putAll(value.asArray, (first, second..last))
	}

	bufnum { arg key;
		var res = this.at(key);
		^if(res.isSequenceableCollection) { res.collect(_.bufnum) } { res.bufnum }
	}

	alloc { arg key, numFrames = 2048, numChannels = 1;
		var buf = this.at(key);
		if(buf.numFrames != numFrames or: {buf.numChannels != numChannels}) {
			buf.numFrames_(numFrames).numChannels_(numChannels).alloc;
		};
		^buf;
	}

	read { arg key, path, startFrame = 0, numFrames = -1, completionMessage;
		if(server.serverRunning.not) { Error("server not running").throw };
		this.at(key).allocRead(path, startFrame, numFrames, completionMessage);
	}

	zero { arg key;
		var buf = envir.at(key);
		buf !? { buf.zero }
	}

	fft { arg key, size, numChannels=1;
		^this.alloc(key, size ? fftsize, numChannels).bufnum;
	}

	cue { arg key, path, startFrame=0, completionMessage;
		var buf = this.at(key);
		if(path.isNil) { path = buf.path };
		^buf.cueSoundFile(path, startFrame, completionMessage)
	}

	bufFrames { arg key;
		^BufFrames.kr(this.at(key).bufnum)
	}

	bufDur { arg key;
		^BufDur.kr(this.at(key).bufnum)
	}

	rateScale { arg key;
		^BufRateScale.kr(this.at(key).bufnum)
	}

}


// to do:


/*
	maybe make it work even if the server is not booted
	consecutive buffers (allocConsecutive returns an array. store array in dict as it is?)

	release info when buffer is freed (shouldn't Buffer do this anyway?)

*/

