
Buffer {

	var <server, <bufnum, <>numFrames = -1, <>numChannels = 1;
	var <path;
	
	// doesn't send
	*new { arg server, numFrames = -1, numChannels=1, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames,
						numChannels)
	}
	

	*alloc { arg server, numFrames, numChannels=1, completionMessage, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames,
						numChannels).alloc(completionMessage)
	}
	allocMsg { arg completionMessage;
		^["/b_alloc", bufnum, numFrames, numChannels, completionMessage.value(this)]
	}
	allocReadMsg { arg argpath,startFrame,completionMessage;
		path = argpath;
		^["/b_allocRead",bufnum, path,startFrame,numFrames, completionMessage.value(this)]
	}

	// read whole file into memory for PlayBuf etc.
	*read { arg server,path,startFrame = 0,numFrames = -1, completionMessage, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames)
					.allocRead(path,startFrame,completionMessage)
	}
	read { arg argpath, fileStartFrame = 0, numFrames = -1, 
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		server.listSendMsg(
			this.readMsg(argpath,fileStartFrame,numFrames,bufStartFrame,
						leaveOpen,completionMessage )
				);
	}
	readMsg { arg argpath, fileStartFrame = 0, numFrames = -1, 
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		this.numFrames = numFrames;
		path = argpath;
		^["/b_read", bufnum, path, fileStartFrame, numFrames, 
			bufStartFrame, leaveOpen.binaryValue, completionMessage.value(this)]
		// doesn't set my numChannels etc.
	}
	
	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server,path,startFrame = 0,numChannels= 2,
			 bufferSize=32768,completionMessage;
		^this.alloc(server,bufferSize,numChannels,{ arg buffer;
						buffer.readMsg(path,startFrame,bufferSize,0,true,completionMessage)
					})
	}
	cueSoundFile { arg path,startFrame = 0, bufferSize=32768,completionMessage;
		server.listSendMsg(
			this.cueSoundFileMsg(path,startFrame,bufferSize,completionMessage)
		)
	}
	cueSoundFileMsg { arg path,startFrame,bufferSize=32768,completionMessage;
		numFrames = bufferSize;
		^["/b_read", bufnum,path,startFrame,bufferSize,0,1,completionMessage.value(this) ]
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
		server.bufferAllocator.free(bufnum);
		^["/b_free", bufnum, completionMessage.value(this)]
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
		server.listSendMsg(["/b_setn",bufnum,startAt,values.size] 
			++ values ++ morePairs.flat);
	}
	get { arg index, action;
		OSCpathResponder(server.addr,['/b_set',bufnum,index],{ arg time, r, msg; 
			action.value(msg.at(3)); r.remove }).add;
		server.listSendMsg(["/b_get",bufnum,index]);
	}
	getn { arg index, count, action;

		OSCpathResponder(server.addr,['/b_setn',bufnum,index],{arg time, r, msg; 
			action.value(msg.copyToEnd(4)); r.remove } ).add; 
		server.listSendMsg(["/b_getn",bufnum,index, count]);
	}
	setnMsg { arg startAt , values ... morePairs;
		^["/b_setn",bufnum,startAt,values.size] 
			++ values ++ morePairs.flat
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
	sine1 { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine1",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	sine2 { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine2",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	sine3 { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"sine3",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	cheby { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		server.listSendMsg(["/b_gen",bufnum,"cheby",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	sine1Msg { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine1",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}
	sine2Msg { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine2",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}
	sine3Msg { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"sine3",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}
	chebyMsg { arg amplitudes,normalize=true,asWavetable=true,clearFirst=true;
		^["/b_gen",bufnum,"cheby",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}
	
	copyTo { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		server.listSendMsg(
			this.copyToMsg(buf, dstStartAt, srcStartAt, numSamples)
		)
	}
	copyToMsg { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
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
		OSCresponder(server.addr,'/b_info',{ arg a,b,c;
			var bb,bufnum,numFrames,numChannels,sr;
			# bb, bufnum,numFrames,numChannels,sr = c;
			Post << "bufnum      :" << bufnum << Char.nl
				<< "numFrames   : " << numFrames << Char.nl
				<< "numChannels : " << numChannels << Char.nl
				<< "sampleRate  :" << sr << Char.nl << Char.nl;
		}).add.removeWhenDone;
		server.sendMsg("/b_query",bufnum) 
	}
	updateInfo {
		OSCresponder(server.addr, "/b_info", { arg t, r, m;
			numFrames = m.at(2);
			numChannels = m.at(3);
		}).add.removeWhenDone;
		server.sendMsg("/b_query", bufnum);
	}
	
	//private	
	alloc { arg completionMessage;
		server.listSendMsg( this.allocMsg(completionMessage) )
	}
	allocRead { arg argpath,startFrame,completionMessage;
		path = argpath;
		server.listSendMsg(this.allocReadMsg( argpath,startFrame,completionMessage));
	}
	
	printOn { arg stream; 
		stream << this.class.name << "(" <<* [bufnum,numFrames,numChannels,path] <<")";
	}

	*loadDialog { arg server,startFrame = 0,numFrames = -1, bufnum;
		server = server ? Server.default;
		^super.newCopyArgs(server,
						bufnum ?? { server.bufferAllocator.alloc(1) },
						numFrames).loadDialog(startFrame)
	}
	loadDialog { arg startFrame, completionMessage;
		File.openDialog("Select a file...",{ arg path;
			this.allocRead(path, startFrame, completionMessage);
		});
	}
}

