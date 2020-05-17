Buffer {

	// don't use the setter methods for the vars below
	// they're private and have no effect on the server
	var <server, <bufnum, <>numFrames, <>numChannels, <>sampleRate;
	var <>path, <>startFrame, >doOnInfo;

	classvar serverCaches;


	*initClass { serverCaches = IdentityDictionary.new }

	// doesn't send
	*new { arg server, numFrames, numChannels, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		^super.newCopyArgs(server,
						bufnum,
						numFrames,
						numChannels).sampleRate_(server.sampleRate).cache
	}

	*alloc { arg server, numFrames, numChannels = 1, completionMessage, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		^super.newCopyArgs(server,
						bufnum,
						numFrames,
						numChannels)
					.alloc(completionMessage).sampleRate_(server.sampleRate).cache
	}

	*allocConsecutive { arg numBufs = 1, server, numFrames, numChannels = 1, completionMessage, bufnum;
		var	bufBase;
		bufBase = bufnum ?? { server.nextBufferNumber(numBufs) };
		numFrames = numFrames.asInteger;
		numChannels = numChannels.asInteger;
		^Array.fill(numBufs, { |i|
			// note, cannot use alloc or allocMsg here
			// because those methods don't pass a buffer index for completion message
			var newBuf = Buffer.new(server, numFrames, numChannels, i + bufBase);
			server.sendMsg(\b_alloc, i + bufBase, numFrames, numChannels,
				completionMessage.value(newBuf, i));
			newBuf.cache
		})
	}

	alloc { arg completionMessage;
		server.listSendMsg( this.allocMsg(completionMessage) )
	}

	allocRead { arg argpath, startFrame = 0, numFrames = -1, completionMessage;
		path = argpath;
		this.startFrame = startFrame;
		server.listSendMsg(this.allocReadMsg( argpath, startFrame, numFrames, completionMessage))
	}

	allocReadChannel { arg argpath, startFrame = 0, numFrames = -1, channels, completionMessage;
		path = argpath;
		this.startFrame = startFrame;
		server.listSendMsg(this.allocReadChannelMsg( argpath, startFrame, numFrames, channels,
			completionMessage))
	}

	allocMsg { arg completionMessage;
		this.cache;
		^["/b_alloc", bufnum, numFrames.asInteger, numChannels.asInteger, completionMessage.value(this)]
	}

	allocReadMsg { arg argpath, startFrame = 0, numFrames = -1, completionMessage;
		this.cache;
		path = argpath;
		this.startFrame = startFrame;
		^["/b_allocRead", bufnum, path, startFrame.asInteger, (numFrames ? -1).asInteger, completionMessage.value(this)]
	}

	allocReadChannelMsg { arg argpath, startFrame = 0, numFrames = -1, channels, completionMessage;
		this.cache;
		path = argpath;
		this.startFrame = startFrame;
		completionMessage !? { completionMessage = [completionMessage.value(this)] };
		^["/b_allocReadChannel", bufnum, path, startFrame.asInteger, (numFrames ? -1).asInteger] ++ channels ++ completionMessage
	}

	// read whole file into memory for PlayBuf etc.
	// adds a query as a completion message
	*read { arg server, path, startFrame = 0, numFrames = -1, action, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		^super.newCopyArgs(server, bufnum)
					.doOnInfo_(action).cache
					.allocRead(path, startFrame, numFrames, {|buf|["/b_query", buf.bufnum] })
	}

	read { arg argpath, fileStartFrame = 0, numFrames = -1, bufStartFrame = 0, leaveOpen = false, action;
		this.cache;
		doOnInfo = action;
		server.listSendMsg(
			this.readMsg(argpath, fileStartFrame, numFrames, bufStartFrame,
						leaveOpen, {|buf| ["/b_query", buf.bufnum] })
		);
	}

	*readChannel { arg server, path, startFrame = 0, numFrames = -1, channels, action, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		^super.newCopyArgs(server, bufnum)
					.doOnInfo_(action).cache
					.allocReadChannel(path, startFrame, numFrames, channels,
						{|buf|["/b_query", buf.bufnum]})
	}

	readChannel { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, channels, action;
		this.cache;
		doOnInfo = action;
		server.listSendMsg(
			this.readChannelMsg(argpath, fileStartFrame, numFrames, bufStartFrame,
						leaveOpen, channels, {|buf| ["/b_query", buf.bufnum] })
		)
	}

	*readNoUpdate { arg server, path, startFrame = 0, numFrames = -1, bufnum, completionMessage;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		^super.newCopyArgs(server, bufnum).allocRead(path, startFrame, numFrames, completionMessage)
	}

	readNoUpdate { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		server.listSendMsg(
			this.readMsg(
				argpath, fileStartFrame, numFrames, bufStartFrame, leaveOpen, completionMessage
			)
		)
	}

	readMsg { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		path = argpath;
		^["/b_read", bufnum, path, fileStartFrame.asInteger, (numFrames ? -1).asInteger,
			bufStartFrame.asInteger, leaveOpen.binaryValue, completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}

	readChannelMsg { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, channels, completionMessage;
		path = argpath;
		^["/b_readChannel", bufnum, path, fileStartFrame.asInteger, (numFrames ? -1).asInteger,
			bufStartFrame.asInteger, leaveOpen.binaryValue] ++ channels ++ [completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}

	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server, path, startFrame = 0, numChannels= 2, bufferSize=32768, completionMessage;
		^this.alloc(server, bufferSize, numChannels, { arg buffer;
			buffer.cueSoundFileMsg(path, startFrame, completionMessage);
		}).cache
	}

	cueSoundFile { arg path, startFrame, completionMessage;
		this.path_(path);
		server.listSendMsg(
			this.cueSoundFileMsg(path, startFrame, completionMessage)
		)
	}

	cueSoundFileMsg { arg path, startFrame = 0, completionMessage;
		^["/b_read", bufnum, path, startFrame.asInteger, numFrames.asInteger, 0, 1, completionMessage.value(this)]
	}

	// transfer a collection of numbers to a buffer through a file
	*loadCollection { arg server, collection, numChannels = 1, action;
		var data, sndfile, path, bufnum, buffer;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		if(server.isLocal, {
			if(collection.isKindOf(RawArray).not) { collection = collection.as(FloatArray) };
			sndfile = SoundFile.new;
			sndfile.sampleRate = server.sampleRate;
			sndfile.numChannels = numChannels;
			path = PathName.tmp ++ sndfile.hash.asString;
			if(sndfile.openWrite(path),
				{
					sndfile.writeData(collection);
					sndfile.close;
					^super.newCopyArgs(server, bufnum)
						.cache.doOnInfo_({ |buf|
							if(File.delete(path), { buf.path = nil},
								{("Could not delete data file:" + path).warn;});
							action.value(buf);
						}).allocRead(path, 0, -1, {|buf| ["/b_query", buf.bufnum] })

				}, { "Failed to write data".warn; ^nil }
			)
		}, { "cannot use loadCollection with a non-local Server".warn; ^nil })
	}

	loadCollection { arg collection, startFrame = 0, action;
		var data, sndfile, path;
		if(server.isLocal, {
			if(collection.isKindOf(RawArray).not,
				{data = collection.collectAs({|item| item}, FloatArray)}, {data = collection;}
			);
			if ( collection.size > ((numFrames - startFrame) * numChannels),
				{ "Collection larger than available number of Frames".warn });
			sndfile = SoundFile.new;
			sndfile.sampleRate = server.sampleRate;
			sndfile.numChannels = numChannels;
			path = PathName.tmp ++ sndfile.hash.asString;
			if(sndfile.openWrite(path),
				{
					sndfile.writeData(data);
					sndfile.close;
					this.read(path, bufStartFrame: startFrame, action: { |buf|
						if(File.delete(path), { buf.path = nil },
							{("Could not delete data file:" + path).warn });
						action.value(buf)
					})

				}, { "Failed to write data".warn });
		}, {"cannot do fromCollection with a non-local Server".warn })
	}

	// send a Collection to a buffer one UDP sized packet at a time
	*sendCollection { arg server, collection, numChannels = 1, wait = -1, action;
		var buffer = this.new(server, ceil(collection.size / numChannels), numChannels);
		forkIfNeeded {
			buffer.alloc;
			server.sync;
			buffer.sendCollection(collection, 0, wait, action);
		}
		^buffer
	}

	sendCollection { arg collection, startFrame = 0, wait = -1, action;
		var collstream, collsize, bundsize;

		if(collection.isSequenceableCollection
		and: { startFrame.isNumber and: { wait.isNumber } }) {
			collstream = CollStream.new;
			collstream.collection = collection;
			collsize = collection.size;

			if ( collsize > ((numFrames - startFrame) * numChannels),
				{ "Collection larger than available number of Frames".warn });

			this.streamCollection(collstream, collsize, startFrame * numChannels, wait, action)
		} {
			MethodError("Invalid arguments to Buffer:sendCollection", this).throw
		}
	}

	// called internally
	streamCollection { arg collstream, collsize, startFrame = 0, wait = -1, action;
		var bundsize, pos;
		{
			// wait = -1 allows an OSC roundtrip between packets
			// wait = 0 might not be safe in a high traffic situation
			// maybe okay with tcp
			pos = collstream.pos;
			while { pos < collsize } {
				// 1626 max size for setn under udp
				bundsize = min(1626, collsize - pos);
				server.listSendMsg(['/b_setn', bufnum, pos + startFrame, bundsize]
					++ Array.fill(bundsize, { collstream.next }));
				pos = collstream.pos;
				if(wait >= 0) { wait.wait } { server.sync };
			};

			action.value(this);

		}.forkIfNeeded
	}

	// these next two get the data and put it in a float array which is passed to action

	loadToFloatArray { arg index = 0, count = -1, action;
		var msg, cond, path, file, array;
		{
			path = PathName.tmp ++ this.hash.asString;
			msg = this.write(path, "aiff", "float", count, index);
			server.sync;
			file = SoundFile.new;
			protect {
				file.openRead(path);
				array = FloatArray.newClear(file.numFrames * file.numChannels);
				file.readData(array);
			} {
				file.close;
				if(File.delete(path).not) { ("Could not delete data file:" + path).warn };
			};

			action.value(array, this);

		}.forkIfNeeded
	}

	// risky without wait
	getToFloatArray { arg index = 0, count = -1, wait = 0.01, timeout = 3, action;
		var refcount, array, pos, getsize, resp, done = false;

		pos = index = index.asInteger;
		// treat -1 and nil the same
		if(count == -1 || count.isNil) {
			count = (numFrames * numChannels).asInteger - index;
		};
		array = FloatArray.newClear(count);
		refcount = (count / 1633).roundUp;
		count = count + pos;

		resp = OSCFunc({ arg msg;
			if(msg[1] == bufnum, {
				array = array.overWrite(FloatArray.newFrom(msg.copyToEnd(4)), msg[2] - index);
				refcount = refcount - 1;
				if(refcount <= 0, { done = true; resp.clear; action.value(array, this) });
			});
		}, '/b_setn', server.addr);

		{
			while { pos < count } {
				// 1633 max size for getn under udp
				getsize = min(1633, count - pos);
				server.listSendMsg(this.getnMsg(pos, getsize));
				pos = pos + getsize;
				if(wait >= 0) { wait.wait } { server.sync };
			};

		}.forkIfNeeded;

		// lose the responder if the network choked
		SystemClock.sched(timeout, {
			if(done.not) {
				resp.free;
				"Buffer-streamToFloatArray failed!".warn;
				"Try increasing wait time".postln
			}
		})
	}

	write { arg path, headerFormat = "aiff", sampleFormat = "int24", numFrames = -1,
						startFrame = 0, leaveOpen = false, completionMessage;
		path = path ?? { thisProcess.platform.recordingsDir +/+ "SC_" ++ Date.localtime.stamp ++ "." ++ headerFormat };
		server.listSendMsg(
			this.writeMsg(path,
				headerFormat, sampleFormat, numFrames, startFrame,
				leaveOpen, completionMessage
			)
		)
	}

	writeMsg { arg path, headerFormat = "aiff", sampleFormat = "int24", numFrames = -1,
						startFrame = 0, leaveOpen = false, completionMessage;
		if(bufnum.isNil) { Error("Cannot write a % that has been freed".format(this.class.name)).throw };
		// doesn't change my path
		^["/b_write", bufnum, path,
				headerFormat, sampleFormat, numFrames.asInteger, startFrame.asInteger,
				leaveOpen.binaryValue, completionMessage.value(this)];
	}

	free { arg completionMessage;
		if(bufnum.isNil) {
			"Cannot call free on a Buffer that has been freed".warn;
			^nil
		} {
			server.listSendMsg(this.freeMsg(completionMessage))
		}
	}

	freeMsg { arg completionMessage;
		var msg;
		if(bufnum.isNil) {
			"Cannot construct a freeMsg for a Buffer that has been freed".warn;
			^nil
		} {
			this.uncache;
			server.bufferAllocator.free(bufnum);
			msg = [\b_free, bufnum, completionMessage.value(this)];
			bufnum = numFrames = numChannels = sampleRate = path = startFrame = nil;
			^msg
		}
	}

	*freeAll { arg server;
		(server ? Server.default).freeAllBuffers;
		this.clearServerCaches(server);
	}

	zero { arg completionMessage;
		server.listSendMsg(this.zeroMsg(completionMessage));
	}

	zeroMsg { arg completionMessage;
		if(bufnum.isNil) { Error("Cannot zero a % that has been freed".format(this.class.name)).throw };
		^[\b_zero, bufnum, completionMessage.value(this)]
	}

	set { arg index, float ... morePairs;
		server.listSendMsg(this.setMsg(index, float, *morePairs));
	}

	setMsg { arg index, float ... morePairs;
		if(bufnum.isNil) { Error("Cannot set values in a % that has been freed".format(this.class.name)).throw };
		^[\b_set, bufnum, index.asInteger, float]
		++ morePairs.collect { |num, i| if(i.even) { num.asInteger } { num } }
	}

	setn { arg ... args;
		server.listSendMsg([\b_setn, bufnum] ++ this.setnMsgArgs(*args))
	}

	setnMsgArgs { arg ... args;
		var nargs;
		nargs = List.new;
		args.pairsDo{ arg control, moreVals;
			if(moreVals.isArray, {
				nargs.addAll([control.asInteger, moreVals.size] ++ moreVals)
			}, {
				nargs.addAll([control.asInteger, 1, moreVals]);
			});
		};
		^nargs
	}

	setnMsg { arg ... args;
		if(bufnum.isNil) { Error("Cannot 'setn' values in a % that has been freed".format(this.class.name)).throw };
		^[\b_setn, bufnum] ++ this.setnMsgArgs(*args)
	}

	get { arg index, action;
		// note: do not try to optimize this by moving 'getMsg' to the end
		// we need 'getMsg' to check the buffer's validity *before* making the OSCFunc
		// 'getMsg' must be first!
		var msg = this.getMsg(index);
		OSCFunc({ |message|
			// The server replies with a message of the form [/b_set, bufnum, index, value].
			// We want "value," which is at index 3.
			action.value(message[3]);
		}, \b_set, server.addr, argTemplate: [bufnum, index.asInteger]).oneShot;
		server.listSendMsg(msg)
	}

	getMsg { arg index;
		if(bufnum.isNil) { Error("Cannot get values from a % that has been freed".format(this.class.name)).throw };
		^[\b_get, bufnum, index.asInteger]
	}

	getn { arg index, count, action;
		// note: do not try to optimize this by moving 'getnMsg' to the end
		// we need 'getnMsg' to check the buffer's validity *before* making the OSCFunc
		// 'getnMsg' must be first!
		var msg = this.getnMsg(index, count);  // action is not used
		OSCFunc({ |message|
			// The server replies with a message of the form
			// [/b_setn, bufnum, starting index, length, ...sample values].
			// We want the sample values, which start at index 4.
			action.value(message[4..]);
		}, \b_setn, server.addr, argTemplate: [bufnum, index]).oneShot;
		server.listSendMsg(msg)
	}

	getnMsg { arg index, count, action;
		if(bufnum.isNil) { Error("Cannot 'getn' values from a % that has been freed".format(this.class.name)).throw };
		^[\b_getn, bufnum, index.asInteger, count.asInteger]
	}

	fill { arg startAt, numFrames, value ... more;
		server.listSendMsg(this.fillMsg(startAt, numFrames, value, *more));
	}

	fillMsg { arg startAt, numFrames, value ... more;
		if(bufnum.isNil) { Error("Cannot fill a % that has been freed".format(this.class.name)).throw };
		^[\b_fill, bufnum, startAt.asInteger, numFrames.asInteger, value]
		++ more.collect { |num, i| if(i % 3 == 2) { num } { num.asInteger } }
	}

	normalize { arg newmax=1, asWavetable=false;
		server.listSendMsg(this.normalizeMsg(newmax, asWavetable))
	}

	normalizeMsg { arg newmax=1, asWavetable=false;
		if(bufnum.isNil) { Error("Cannot normalize a % that has been freed".format(this.class.name)).throw };
		^[\b_gen, bufnum, if(asWavetable, "wnormalize", "normalize"), newmax]
	}

	gen { arg genCommand, genArgs, normalize=true, asWavetable=true, clearFirst=true;
		server.listSendMsg(this.genMsg(genCommand, genArgs, normalize, asWavetable, clearFirst))
	}

	genMsg { arg genCommand, genArgs, normalize=true, asWavetable=true, clearFirst=true;
		if(bufnum.isNil) { Error("Cannot use 'gen' messages on a % that has been freed".format(this.class.name)).throw };
		^[\b_gen, bufnum, genCommand,
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ genArgs
	}

	sine1 { arg amps, normalize=true, asWavetable=true, clearFirst=true;
		server.listSendMsg(this.sine1Msg(amps, normalize, asWavetable, clearFirst))
	}

	sine2 { arg freqs, amps, normalize=true, asWavetable=true, clearFirst=true;
		server.listSendMsg(this.sine2Msg(freqs, amps, normalize, asWavetable, clearFirst))
	}

	sine3 { arg freqs, amps, phases, normalize=true, asWavetable=true, clearFirst=true;
		server.listSendMsg(this.sine3Msg(freqs, amps, phases, normalize, asWavetable, clearFirst))
	}

	cheby { arg amps, normalize=true, asWavetable=true, clearFirst=true;
		server.listSendMsg(this.chebyMsg(amps, normalize, asWavetable, clearFirst))
	}

	sine1Msg { arg amps, normalize=true, asWavetable=true, clearFirst=true;
		if(bufnum.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\b_gen, bufnum, "sine1",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amps
	}

	sine2Msg { arg freqs, amps, normalize=true, asWavetable=true, clearFirst=true;
		if(bufnum.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\b_gen, bufnum, "sine2",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps].lace(freqs.size * 2)
	}

	sine3Msg { arg freqs, amps, phases, normalize=true, asWavetable=true, clearFirst=true;
		if(bufnum.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\b_gen, bufnum, "sine3",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps, phases].lace(freqs.size * 3)
	}

	chebyMsg { arg amps, normalize=true, asWavetable=true, clearFirst=true;
		if(bufnum.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\b_gen, bufnum, "cheby",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amps
	}

	copyData { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		server.listSendMsg(this.copyMsg(buf, dstStartAt, srcStartAt, numSamples))
	}

	copyMsg { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		if(bufnum.isNil) { Error("Cannot copy a % that has been freed".format(this.class.name)).throw };
		^[\b_gen, buf.bufnum, "copy", dstStartAt.asInteger, bufnum, srcStartAt.asInteger, numSamples.asInteger]
	}

	// close a file, write header, after DiskOut usage
	close { arg completionMessage;
		server.listSendMsg(this.closeMsg(completionMessage))
	}

	closeMsg { arg completionMessage;
		if(bufnum.isNil) { Error("Cannot close a % that has been freed".format(this.class.name)).throw };
		^[\b_close, bufnum, completionMessage.value(this) ]
	}

	query { |action|
		// as above, make sure 'queryMsg' runs before creating the OSCFunc
		var msg = this.queryMsg;
		action = action ?? {
			{ |addr, bufnum, numFrames, numChannels, sampleRate|
				"bufnum: %\nnumFrames: %\nnumChannels: %\nsampleRate: %\n".format(
					bufnum, numFrames, numChannels, sampleRate
				).postln;
			}
		};
		OSCFunc({ |msg|
			action.valueArray(msg)
		}, \b_info, server.addr, nil, [bufnum]).oneShot;
		server.listSendMsg(msg)
	}

	queryMsg {
		if(bufnum.isNil) { Error("Cannot query a % that has been freed".format(this.class.name)).throw };
		^[\b_query, bufnum]
	}

	updateInfo { arg action;
		// add to the array here. That way, update will be accurate even if this buf
		// has been freed
		this.cache;
		doOnInfo = action;
		server.listSendMsg(this.queryMsg)
	}

	// cache Buffers for easy info updating
	cache {
		Buffer.initServerCache(server);
		serverCaches[server][bufnum] = this;
	}

	uncache {
		if(serverCaches[server].notNil, {
			serverCaches[server].removeAt(bufnum);
		});
		if(serverCaches[server].size == 1) {
			// the 1 item would be the responder
			// if there is more than 1 item then the rest are cached buffers
			// else we can remove.
			// cx: tho i don't see why its important. it will just have to be added
			// back when the next buffer is added and the responder is removed when
			// the server reboots
			Buffer.clearServerCaches(server);
		}
	}

	*initServerCache { arg server;
		serverCaches[server] ?? {
			serverCaches[server] = IdentityDictionary.new;
			serverCaches[server][\responder] = OSCFunc({ |m|
				var	buffer = serverCaches[server][m[1]];
				if(buffer.notNil) {
					buffer.numFrames = m[2];
					buffer.numChannels = m[3];
					buffer.sampleRate = m[4];
					buffer.queryDone;
				};
			}, \b_info, server.addr).fix;
			NotificationCenter.register(server, \newAllocators, this, {
				this.clearServerCaches(server);
			});
		}
	}

	*clearServerCaches { arg server;
		if(serverCaches[server].notNil) {
			serverCaches[server][\responder].free;
			serverCaches.removeAt(server);
		}
	}

	*cachedBuffersDo { arg server, func;
		var	i = 0;
		serverCaches[server] !? {
			serverCaches[server].keysValuesDo({ |key, value|
				if(key.isNumber) { func.value(value, i); i = i + 1 };
			})
		}
	}

	*cachedBufferAt { arg server, bufnum;
		^serverCaches[server].tryPerform(\at, bufnum)
	}

	// called from Server when b_info is received
	queryDone {
		doOnInfo.value(this);
		doOnInfo = nil;
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* [bufnum, numFrames, numChannels, sampleRate, path] <<")"
	}

	*loadDialog { arg server, startFrame = 0, numFrames, action, bufnum;
		var buffer;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.nextBufferNumber(1) };
		buffer = super.newCopyArgs(server, bufnum).cache;
		Dialog.openPanel({ arg path;
			buffer.doOnInfo_(action)
				.allocRead(path, startFrame, numFrames, { ["/b_query", buffer.bufnum] })
		});
		^buffer
	}

	play { arg loop = false, mul = 1;
		if(bufnum.isNil) { Error("Cannot play a % that has been freed".format(this.class.name)).throw };
		^{ var player;
			player = PlayBuf.ar(numChannels, bufnum, BufRateScale.kr(bufnum),
				loop: loop.binaryValue);
			if(loop.not, FreeSelfWhenDone.kr(player));
			player * mul;
		}.play(server)
	}

	duration { ^numFrames / sampleRate }

	asUGenInput { ^this.bufnum }
	asControlInput { ^this.bufnum }

	asBufWithValues {
		^this
	}
}
