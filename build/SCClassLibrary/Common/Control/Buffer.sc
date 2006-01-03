
Buffer {

	// don't use the setter methods for the vars below
	// they're private and have no effect on the server
	var <server, <bufnum, <>numFrames, <>numChannels, <>sampleRate;
	var <>path, >doOnInfo;
	
	// doesn't send
	*new { arg server, numFrames, numChannels, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames,
						numChannels).sampleRate_(server.sampleRate);
	}

	*alloc { arg server, numFrames, numChannels = 1, completionMessage, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames,
						numChannels)
					.alloc(completionMessage).sampleRate_(server.sampleRate);
	}
	
	*allocConsecutive { |numBufs = 1, server, numFrames, numChannels = 1, completionMessage,
			bufnum|
		var	bufBase, newBuf;
		bufBase = bufnum ?? { server.bufferAllocator.alloc(numBufs) };
		^Array.fill(numBufs, { |i|
			newBuf = Buffer.new(server, numFrames, numChannels, i + bufBase);
			server.sendMsg(\b_alloc, i + bufBase, numFrames, numChannels,
				completionMessage.value(newBuf, i));
			newBuf
		});
	}

	alloc { arg completionMessage;
		server.listSendMsg( this.allocMsg(completionMessage) )
	}
	allocRead { arg argpath,startFrame,numFrames, completionMessage;
		path = argpath;
		server.listSendMsg(this.allocReadMsg( argpath,startFrame,numFrames, completionMessage));
	}
	
	allocMsg { arg completionMessage;
		this.addToServerArray;
		^["/b_alloc", bufnum, numFrames, numChannels, completionMessage.value(this)]
	}
	allocReadMsg { arg argpath,startFrame,numFrames, completionMessage;
		this.addToServerArray;
		path = argpath;
		^["/b_allocRead",bufnum, path,startFrame,numFrames ? -1, completionMessage.value(this)]
	}

	// read whole file into memory for PlayBuf etc.
	// adds a query as a completion message
	*read { arg server,path,startFrame = 0,numFrames, action, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) })
					.doOnInfo_(action).waitForBufInfo
					.allocRead(path,startFrame,numFrames,{|buf|["/b_query",buf.bufnum]});
	}
	read { arg argpath, fileStartFrame = 0, numFrames, 
					bufStartFrame = 0, leaveOpen = false, action;
		this.addToServerArray;
		doOnInfo = action;
		this.waitForBufInfo;
		server.listSendMsg(
			this.readMsg(argpath,fileStartFrame,numFrames,bufStartFrame,
						leaveOpen,{|buf|["/b_query",buf.bufnum]} )
		);
	}
	*readNoUpdate { arg server,path,startFrame = 0,numFrames, bufnum, completionMessage;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) })
					.allocRead(path,startFrame,numFrames, completionMessage);
	}
	readNoUpdate { arg argpath, fileStartFrame = 0, numFrames, 
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		server.listSendMsg(
			this.readMsg(argpath,fileStartFrame,numFrames,bufStartFrame, 
				leaveOpen, completionMessage)
		);
	}
	readMsg { arg argpath, fileStartFrame = 0, numFrames, 
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		path = argpath;
		^["/b_read", bufnum, path, fileStartFrame, numFrames ? -1, 
			bufStartFrame, leaveOpen.binaryValue, completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}
	
	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server,path,startFrame = 0,numChannels= 2,
			 bufferSize=32768,completionMessage;
		^this.alloc(server,bufferSize,numChannels,{ arg buffer;
						buffer.readMsg(path,startFrame,bufferSize,0,true,completionMessage)
					}).addToServerArray;
	}
	
	cueSoundFile { arg path,startFrame,completionMessage;
		server.listSendMsg(
			this.cueSoundFileMsg(path,startFrame,completionMessage)
		)
	}
	cueSoundFileMsg { arg path,startFrame = 0,completionMessage;
		^["/b_read", bufnum,path,startFrame,numFrames,0,1,completionMessage.value(this) ]
	}
	
	// transfer a collection of numbers to a buffer through a file
	*loadCollection { arg server, collection, numChannels = 1, action;
		var data, sndfile, path, bufnum, buffer;
		server = server ? Server.default;
		server.isLocal.if({
			if(collection.isKindOf(RawArray).not, 
				{data = collection.collectAs({|item| item}, FloatArray)}, {data = collection;}
			);
			sndfile = SoundFile.new;
			sndfile.sampleRate = server.sampleRate;
			sndfile.numChannels = numChannels;
			path = PathName.tmp ++ data.hash.asString;
			if(sndfile.openWrite(path),
				{
					sndfile.writeData(data);
					sndfile.close;
					^super.newCopyArgs(server, server.bufferAllocator.alloc(1))
						.addToServerArray.doOnInfo_({ |buf|
							if(File.delete(path), { buf.path = nil}, 
								{("Could not delete data file:" + path).warn;});
							action.value(buf);
						}).waitForBufInfo.allocRead(path, 0, -1,{|buf|["/b_query",buf.bufnum]});
				
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
			if ( collection.size > (numFrames - startFrame), 
				{ "Collection larger than available number of Frames".warn });
			sndfile = SoundFile.new;
			sndfile.sampleRate = server.sampleRate;
			sndfile.numChannels = numChannels;
			path = PathName.tmp ++ data.hash.asString;
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
	*sendCollection { arg server, collection, numChannels, wait = 0.0, action;
		var collstream, buffer, collsize, bufnum, bundsize, pos;
		
		collstream = CollStream.new;
		collstream.collection = collection;
		collsize = collection.size;
		server = server ? Server.default;
		bufnum = server.bufferAllocator.alloc(1);
		buffer = super.newCopyArgs(server, bufnum, collsize, numChannels)
			.addToServerArray.sampleRate_(server.sampleRate);
		
		// first send with alloc
		// 1626 largest setn size with an alloc
		bundsize = min(1626, collsize - collstream.pos);
		server.listSendMsg(buffer.allocMsg({["b_setn", bufnum, 0, bundsize] 
			++ Array.fill(bundsize, {collstream.next})}));

		buffer.streamCollection(collstream, collsize, 0, wait, action);
		^buffer;
	}
	
	sendCollection { arg collection, startFrame = 0, wait = 0.0, action;
		var collstream, collsize, bundsize;
		
		collstream = CollStream.new;
		collstream.collection = collection;
		collsize = collection.size;
		
		if ( collsize > (numFrames - startFrame), 
				{ "Collection larger than available number of Frames".warn });
		
		this.streamCollection(collstream, collsize, startFrame, wait, action);
	}
	
	// called internally
	streamCollection { arg collstream, collsize, startFrame, wait, action;
		var bundsize, pos;
		{ 
			// wait = 0 might not be safe in a high traffic situation
			// maybe okay with tcp
			pos = collstream.pos;
			while({pos < collsize}, {
				wait.wait;
				// 1633 max size for setn under udp 
				bundsize = min(1633, collsize - pos);
				server.listSendMsg(["b_setn", bufnum, pos + startFrame, bundsize] 
					++ Array.fill(bundsize, {collstream.next}));
				pos = collstream.pos;
			});
			
			action.value(this);

		}.fork(SystemClock);
	}
	
	// these next two get the data and put it in a float array which is passed to action
	
	loadToFloatArray { arg index = 0, count = -1, action;
		var msg, cond, path, file, array;
		Routine.run({
			cond = Condition.new;
			path = PathName.tmp ++ this.hash.asString;
			msg = this.writeMsg(path, "aiff", "float", count, index);
			server.sendMsgSync(cond, *msg);
			file = SoundFile.new;
			file.openRead(path);
			array = FloatArray.newClear(file.numFrames * file.numChannels);
			file.readData(array);
			file.close;
			if(File.delete(path).not, {("Could not delete data file:" + path).warn;});
			action.value(array, this);
		});
	}
	
	// risky without wait 
	getToFloatArray { arg index = 0, count, wait = 0.01, timeout = 3, action;
		var refcount, array, pos = 0, getsize, resp, done = false;
		count = count ? (numFrames * numChannels);
		array = FloatArray.newClear(count);
		refcount = (count / 1633).asInteger + 1;
		resp = OSCresponderNode(server.addr, '/b_setn', { arg time, responder, msg; 
			if(msg[1] == bufnum, {
				//("received" + msg).postln;
				array = array.overWrite(FloatArray.newFrom(msg.copyToEnd(4)), msg[2]);
				refcount = refcount - 1;
				//("countDown" + refcount).postln;
				if(refcount <= 0, {done = true; responder.remove; action.value(array, this); });
			});
		}).add;
		{
			while({pos < count}, { 
				// 1633 max size for getn under udp 
				getsize = min(1633, count - pos);
				//("sending from" + pos).postln;
				server.listSendMsg(this.getnMsg(pos, getsize));
				pos = pos + getsize;			
				wait.wait; 
			});
			
		}.fork;
		// lose the responder if the network choked
		SystemClock.sched(timeout, 
			{ done.not.if({ resp.remove; "Buffer-streamToFloatArray failed!".warn; 
				"Try increasing wait time".postln;}); 
		});
	}
	
	write { arg path,headerFormat="aiff",sampleFormat="int24",numFrames = -1,
						startFrame = 0,leaveOpen = false, completionMessage;
		server.listSendMsg( 
			this.writeMsg(path,headerFormat,sampleFormat,numFrames,startFrame,
				leaveOpen,completionMessage) 
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
		server.freeBuf(bufnum);
		server.bufferAllocator.free(bufnum);
		^["/b_free", bufnum, completionMessage.value(this)];
	}
	*freeAll { arg server;
		var b;
		server = server ? Server.default;
		server.bufferAllocator.blocks.do({ arg block;
			b = b.add( ["/b_free",block.address] );
			server.bufferAllocator.free(block.address);	 
		});
		server.sendBundle(b);
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
	
	setn { arg startAt , values ... morePairs;
		server.sendMsg(*this.setnMsg(startAt, values, *morePairs));
	}
	setnMsg { arg startAt , values ... morePairs;
		var nargs;
		nargs = List.new;
		morePairs.pairsDo({ arg control, moreVals; 
			nargs.addAll([control, moreVals.size, moreVals].flat)}
		);
		^["/b_setn",bufnum,startAt,values.size] 
			++ values ++ nargs;
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
		server.listSendMsg(["/b_fill",bufnum,startAt,numFrames,value]
			 ++ more);
	}
	fillMsg { arg startAt,numFrames,value ... more;
		^["/b_fill",bufnum,startAt,numFrames,value]
			 ++ more;
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
	
	copy { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
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
		OSCresponderNode(server.addr,'/b_info',{ arg time,responder,msg;
			Post << "bufnum      :" << msg[1] << Char.nl
				<< "numFrames   : " << msg[2] << Char.nl
				<< "numChannels : " << msg[3] << Char.nl
				<< "sampleRate  :" << msg[4] << Char.nl << Char.nl;
			responder.remove;
		}).add;
		server.sendMsg("/b_query",bufnum) 
	}

	updateInfo { |action|
		// add to the array here. That way, update will be accurate even if this buf
		// has been freed
		this.addToServerArray;
		doOnInfo = action;
		this.waitForBufInfo;
		server.sendMsg("/b_query", bufnum);
	}
	
	// cache Buffers in an Array for easy info updating
	addToServerArray {
		this.server.addBuf(this);
	}
	// tell the server to wait for a b_info
	waitForBufInfo {
		this.server.waitForBufInfo;
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
		buffer = super.newCopyArgs(server, bufnum ?? { server.bufferAllocator.alloc(1) })
					.addToServerArray;
		File.openDialog("Select a file...",{ arg path;
			buffer.doOnInfo_(action).waitForBufInfo
				.allocRead(path,startFrame,numFrames, {["/b_query",buffer.bufnum]})
		});
		^buffer;
	}
	
	play { arg loop = false, mul = 1;
		^{ var player;
			player = PlayBuf.ar(numChannels,bufnum,BufRateScale.kr(bufnum),loop: loop.binaryValue) 
				* mul; 
			loop.not.if(FreeSelfWhenDone.kr(player));
			player; 
		}.play;
	}
}

