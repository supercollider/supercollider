Buffer {
	classvar	serverCaches;

	// don't use the setter methods for the vars below
	// they're private and have no effect on the server
	var <server, <bufnum, <>numFrames, <>numChannels, <>sampleRate;
	var <>path, >doOnInfo;

	*initClass { serverCaches = IdentityDictionary.new }

	// doesn't send
	*new { arg server, numFrames, numChannels, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		^super.newCopyArgs(server,
						bufnum,
						numFrames,
						numChannels).sampleRate_(server.sampleRate).cache;
	}

	*alloc { arg server, numFrames, numChannels = 1, completionMessage, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		^super.newCopyArgs(server,
						bufnum,
						numFrames,
						numChannels)
					.alloc(completionMessage).sampleRate_(server.sampleRate).cache;
	}

	*allocConsecutive { |numBufs = 1, server, numFrames, numChannels = 1, completionMessage,
			bufnum|
		var	bufBase, newBuf;
		bufBase = bufnum ?? { server.bufferAllocator.alloc(numBufs) };
		if(bufBase.isNil) {
			Error("No block of % consecutive buffer numbers is available.".format(numBufs)).throw
		};
		^Array.fill(numBufs, { |i|
			newBuf = Buffer.new(server, numFrames, numChannels, i + bufBase);
			server.sendMsg(\b_alloc, i + bufBase, numFrames, numChannels,
				completionMessage.value(newBuf, i));
			newBuf.cache
		});
	}

	alloc { arg completionMessage;
		server.listSendMsg( this.allocMsg(completionMessage) )
	}
	allocRead { arg argpath,startFrame = 0,numFrames = -1, completionMessage;
		path = argpath;
		server.listSendMsg(this.allocReadMsg( argpath,startFrame,numFrames, completionMessage));
	}
	allocReadChannel { arg argpath,startFrame,numFrames = 0, channels = -1, completionMessage;
		path = argpath;
		server.listSendMsg(this.allocReadChannelMsg( argpath,startFrame,numFrames, channels,
			completionMessage));
	}
	allocMsg { arg completionMessage;
		this.cache;
		^["/b_alloc", bufnum, numFrames.asInt, numChannels, completionMessage.value(this)]
	}
	allocReadMsg { arg argpath,startFrame = 0,numFrames = -1, completionMessage;
		this.cache;
		path = argpath;
		^["/b_allocRead",bufnum, path,startFrame,(numFrames ? -1).asInt, completionMessage.value(this)]
	}
	allocReadChannelMsg { arg argpath,startFrame = 0,numFrames = -1, channels, completionMessage;
		this.cache;
		path = argpath;
		^["/b_allocReadChannel",bufnum, path,startFrame, (numFrames ? -1).asInt] ++ channels ++ 			[completionMessage.value(this)]
	}

	// read whole file into memory for PlayBuf etc.
	// adds a query as a completion message
	*read { arg server,path,startFrame = 0,numFrames = -1, action, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		^super.newCopyArgs(server, bufnum)
					.doOnInfo_(action).cache
					.allocRead(path,startFrame,numFrames,{|buf|["/b_query",buf.bufnum]});
	}
	read { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, action;
		this.cache;
		doOnInfo = action;
		server.listSendMsg(
			this.readMsg(argpath,fileStartFrame,numFrames,bufStartFrame,
						leaveOpen,{|buf|["/b_query",buf.bufnum]} )
		);
	}
	*readChannel { arg server,path,startFrame = 0,numFrames = -1, channels, action, bufnum;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		^super.newCopyArgs(server, bufnum)
					.doOnInfo_(action).cache
					.allocReadChannel(path,startFrame,numFrames,channels,
						{|buf|["/b_query",buf.bufnum]});
	}
	readChannel { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, channels, action;
		this.cache;
		doOnInfo = action;
		server.listSendMsg(
			this.readChannelMsg(argpath,fileStartFrame,numFrames,bufStartFrame,
						leaveOpen,channels,{|buf|["/b_query",buf.bufnum]} )
		);
	}
	*readNoUpdate { arg server,path,startFrame = 0,numFrames = -1, bufnum, completionMessage;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		^super.newCopyArgs(server, bufnum)
					.allocRead(path,startFrame,numFrames, completionMessage);
	}
	readNoUpdate { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		server.listSendMsg(
			this.readMsg(argpath,fileStartFrame,numFrames,bufStartFrame,
				leaveOpen, completionMessage)
		);
	}
	readMsg { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		path = argpath;
		^["/b_read", bufnum, path, fileStartFrame, (numFrames ? -1).asInt,
			bufStartFrame, leaveOpen.binaryValue, completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}

	readChannelMsg { arg argpath, fileStartFrame = 0, numFrames = -1,
					bufStartFrame = 0, leaveOpen = false, channels, completionMessage;
		path = argpath;
		^["/b_readChannel", bufnum, path, fileStartFrame, (numFrames ? -1).asInt,
			bufStartFrame, leaveOpen.binaryValue] ++ channels ++ [completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}

	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server,path,startFrame = 0,numChannels= 2,
			 bufferSize=32768,completionMessage;
		^this.alloc(server,bufferSize,numChannels,{ arg buffer;
						buffer.readMsg(path,startFrame,bufferSize,0,true,completionMessage)
					}).cache;
	}

	cueSoundFile { arg path,startFrame,completionMessage;
		this.path_(path);
		server.listSendMsg(
			this.cueSoundFileMsg(path,startFrame,completionMessage)
		)
	}
	cueSoundFileMsg { arg path,startFrame = 0,completionMessage;
		^["/b_read", bufnum,path,startFrame,numFrames.asInt,0,1,completionMessage.value(this) ]
	}

	// transfer a collection of numbers to a buffer through a file
	*loadCollection { arg server, collection, numChannels = 1, action;
		var data, sndfile, path, bufnum, buffer;
		server = server ? Server.default;
		bufnum = server.bufferAllocator.alloc(1);
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		server.isLocal.if({
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
						}).allocRead(path, 0, -1,{|buf|["/b_query",buf.bufnum]});

				}, {"Failed to write data".warn; ^nil}
			);
		}, {"cannot use loadCollection with a non-local Server".warn; ^nil});
	}

	loadCollection { arg collection, startFrame = 0, action;
		var data, sndfile, path;
		server.isLocal.if({
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
							{("Could not delete data file:" + path).warn;});
						action.value(buf);
					});

				}, {"Failed to write data".warn;}
			);
		}, {"cannot do fromCollection with a non-local Server".warn;});
	}

	// send a Collection to a buffer one UDP sized packet at a time
	*sendCollection { arg server, collection, numChannels = 1, wait = -1, action;
		var buffer = this.new(server, ceil(collection.size / numChannels), numChannels);
		forkIfNeeded {
			buffer.alloc;
			server.sync;
			buffer.sendCollection(collection, 0, wait, action);
		}
		^buffer;
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

			this.streamCollection(collstream, collsize, startFrame * numChannels, wait, action);
		} {
			MethodError("Invalid arguments to Buffer:sendCollection", this).throw;
		};
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

		}.forkIfNeeded;
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

		}.forkIfNeeded;
	}

	// risky without wait
	getToFloatArray { arg index = 0, count, wait = 0.01, timeout = 3, action;
		var refcount, array, pos, getsize, resp, done = false;
		pos = index = index.asInteger;
		count = (count ? (numFrames * numChannels)).asInteger;
		array = FloatArray.newClear(count);
		refcount = (count / 1633).roundUp;
		count = count + pos;
		//("refcount" + refcount).postln;
		resp = OSCFunc({ arg msg;
			if(msg[1] == bufnum, {
				//("received" + msg).postln;
				array = array.overWrite(FloatArray.newFrom(msg.copyToEnd(4)), msg[2] - index);
				refcount = refcount - 1;
				//("countDown" + refcount).postln;
				if(refcount <= 0, {done = true; resp.clear; action.value(array, this); });
			});
		}, '/b_setn', server.addr);
		{
			while({pos < count}, {
				// 1633 max size for getn under udp
				getsize = min(1633, count - pos);
				//("sending from" + pos).postln;
				server.listSendMsg(this.getnMsg(pos, getsize));
				pos = pos + getsize;
				if(wait >= 0) { wait.wait } { server.sync };
			});

		}.forkIfNeeded;
		// lose the responder if the network choked
		SystemClock.sched(timeout,
			{ done.not.if({ resp.free; "Buffer-streamToFloatArray failed!".warn;
				"Try increasing wait time".postln;});
		});
	}

	write { arg path, headerFormat = "aiff", sampleFormat = "int24", numFrames = -1,
						startFrame = 0,leaveOpen = false, completionMessage;
		path = path ?? { thisProcess.platform.recordingsDir +/+ "SC_" ++ Date.localtime.stamp ++ "." ++ headerFormat };
		server.listSendMsg(
			this.writeMsg(path, headerFormat, sampleFormat, numFrames, startFrame,
				leaveOpen, completionMessage)
			);
	}
	writeMsg { arg path,headerFormat="aiff",sampleFormat="int24",numFrames = -1,
						startFrame = 0,leaveOpen = false, completionMessage;
		// doesn't change my path
		^["/b_write", bufnum, path,
				headerFormat,sampleFormat, numFrames, startFrame,
				leaveOpen.binaryValue, completionMessage.value(this)];
		// writeEnabled = true;
	}

	free { arg completionMessage;
		server.listSendMsg( this.freeMsg(completionMessage) );
	}
	freeMsg { arg completionMessage;
		var msg;
		this.uncache;
		server.bufferAllocator.free(bufnum);
		msg = ["/b_free", bufnum, completionMessage.value(this)];
		bufnum = numFrames = numChannels = sampleRate = path = nil;
		^msg
	}
	*freeAll { arg server;
		var b;
		server = server ? Server.default;
		server.bufferAllocator.blocks.do({ arg block;
			(block.address .. block.address + block.size - 1).do({ |i|
				b = b.add( ["/b_free", i] );
			});
			server.bufferAllocator.free(block.address);
		});
		server.sendBundle(nil, *b);
		this.clearServerCaches(server);
	}
	zero { arg completionMessage;
		server.listSendMsg(this.zeroMsg(completionMessage));
	}
	zeroMsg { arg completionMessage;
		^["/b_zero", bufnum ,  completionMessage.value(this) ]
	}

	set { arg index,float ... morePairs;
		server.listSendMsg(["/b_set",bufnum,index,float] ++ morePairs);
	}
	setMsg { arg index,float ... morePairs;
		^["/b_set",bufnum,index,float] ++ morePairs;
	}

	setn { arg ... args;
		server.sendMsg(*this.setnMsg(*args));
	}
	setnMsgArgs{arg ... args;
		var nargs;
		nargs = List.new;
		args.pairsDo{ arg control, moreVals;
			if(moreVals.isArray,{
				nargs.addAll([control, moreVals.size]++ moreVals)
			},{
				nargs.addAll([control, 1, moreVals]);
			});
		};
		^nargs;
	}
	setnMsg { arg ... args;
		^["/b_setn",bufnum] ++ this.setnMsgArgs(*args);
	}
	get { arg index, action;
		OSCpathResponder(server.addr,['/b_set',bufnum,index],{ arg time, r, msg;
			action.value(msg.at(3)); r.remove }).add;
		server.listSendMsg(["/b_get",bufnum,index]);
	}
	getMsg { arg index;
		^["/b_get",bufnum,index];
	}
	getn { arg index, count, action;
		OSCpathResponder(server.addr,['/b_setn',bufnum,index],{arg time, r, msg;
			action.value(msg.copyToEnd(4)); r.remove } ).add;
		server.listSendMsg(["/b_getn",bufnum,index, count]);
	}
	getnMsg { arg index, count;
		^["/b_getn",bufnum,index, count];
	}


	fill { arg startAt,numFrames,value ... more;
		server.listSendMsg(["/b_fill",bufnum,startAt,numFrames.asInt,value]
			 ++ more);
	}
	fillMsg { arg startAt,numFrames,value ... more;
		^["/b_fill",bufnum,startAt,numFrames.asInt,value]
			 ++ more;
	}

	normalize { arg newmax=1, asWavetable=false;
		server.listSendMsg(["/b_gen",bufnum,if(asWavetable, "wnormalize", "normalize"),newmax]);
	}
	normalizeMsg { arg newmax=1, asWavetable=false;
		^["/b_gen",bufnum,if(asWavetable, "wnormalize", "normalize"),newmax];
	}

	gen { arg genCommand, genArgs, normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,genCommand,
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ genArgs)
	}
	genMsg { arg genCommand, genArgs, normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,genCommand,
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ genArgs;
	}
	sine1 { arg amps,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine1",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amps)
	}
	sine2 { arg freqs, amps,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine2",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps].lace(freqs.size * 2))
	}
	sine3 { arg freqs, amps, phases,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine3",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps, phases].lace(freqs.size * 3))
	}
	cheby { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"cheby",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	sine1Msg { arg amps,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine1",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amps
	}
	sine2Msg { arg freqs, amps,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine2",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps].lace(freqs.size * 2)
	}
	sine3Msg { arg freqs, amps, phases,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine3",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ [freqs, amps, phases].lace(freqs.size * 3)
	}
	chebyMsg { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"cheby",
			normalize.binaryValue
			+ (asWavetable.binaryValue * 2)
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}

	copyData { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		server.listSendMsg(
			this.copyMsg(buf, dstStartAt, srcStartAt, numSamples)
		)
	}
	copyMsg { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		^["/b_gen", buf.bufnum, "copy", dstStartAt, bufnum, srcStartAt, numSamples]
	}

	// close a file, write header, after DiskOut usage
	close { arg completionMessage;
		server.listSendMsg( this.closeMsg(completionMessage)  );
	}
	closeMsg { arg completionMessage;
		^["/b_close", bufnum, completionMessage.value(this) ];
	}

	query {
		OSCFunc({ arg msg;
			Post << "bufnum      : " << msg[1] << Char.nl
				<< "numFrames   : " << msg[2] << Char.nl
				<< "numChannels : " << msg[3] << Char.nl
				<< "sampleRate  : " << msg[4] << Char.nl << Char.nl;
		}, '/b_info', server.addr).oneShot;
		server.sendMsg("/b_query",bufnum)
	}

	updateInfo { |action|
		// add to the array here. That way, update will be accurate even if this buf
		// has been freed
		this.cache;
		doOnInfo = action;
		server.sendMsg("/b_query", bufnum);
	}

	// cache Buffers for easy info updating
	cache {
		Buffer.initServerCache(server);
		serverCaches[server][bufnum] = this;
	}
	uncache {
		if(serverCaches[server].notNil,{
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
	*initServerCache { |server|
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
			}, '/b_info', server.addr).fix;
			NotificationCenter.register(server,\newAllocators,this,{
				this.clearServerCaches(server);
			});
		}
	}
	*clearServerCaches { |server|
		if(serverCaches[server].notNil) {
			serverCaches[server][\responder].free;
			serverCaches.removeAt(server);
		}
	}
	*cachedBuffersDo { |server, func|
		var	i = 0;
		serverCaches[server] !? {
			serverCaches[server].keysValuesDo({ |key, value|
				if(key.isNumber) { func.value(value, i); i = i + 1 };
			});
		}
	}
	*cachedBufferAt { |server, bufnum|
		^serverCaches[server].tryPerform(\at, bufnum)
	}

	// called from Server when b_info is received
	queryDone {
		doOnInfo.value(this);
		doOnInfo = nil;
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* [bufnum,numFrames,numChannels,sampleRate,path] <<")";
	}

	*loadDialog { arg server,startFrame = 0,numFrames, action, bufnum;
		var buffer;
		server = server ? Server.default;
		bufnum ?? { bufnum = server.bufferAllocator.alloc(1) };
		if(bufnum.isNil) {
			Error("No more buffer numbers -- free some buffers before allocating more.").throw
		};
		buffer = super.newCopyArgs(server, bufnum).cache;
		File.openDialog("Select a file...",{ arg path;
			buffer.doOnInfo_(action)
				.allocRead(path,startFrame,numFrames, {["/b_query",buffer.bufnum]})
		});
		^buffer;
	}

	play { arg loop = false, mul = 1;
		^{ var player;
			player = PlayBuf.ar(numChannels,bufnum,BufRateScale.kr(bufnum),
				loop: loop.binaryValue);
			loop.not.if(FreeSelfWhenDone.kr(player));
			player * mul;
		}.play(server);
	}

	duration { ^numFrames / sampleRate }

	asUGenInput { ^this.bufnum }
	asControlInput { ^this.bufnum }

	asBufWithValues {
		^this
	}
}
