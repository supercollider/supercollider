
Buffer {

	var <server, <bufnum, <>numFrames = -1, <>numChannels = 1;
	var <path;
	
	// doesn't send
	*new { arg server,numFrames = -1,numChannels=1;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames,
						numChannels)
	}
	

	*alloc { arg server,numFrames,numChannels=1,completionMessage;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
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
	*read { arg server,path,startFrame = 0,numFrames = -1, completionMessage;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
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
	
	sine1 { arg amplitudes,normalize=false,asWavetable=false,clearFirst=false;
		server.listSendMsg(["/b_gen",bufnum,"sine1",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	sine1Msg { arg amplitudes,normalize=false,asWavetable=false,clearFirst=false;
		^["/b_gen",bufnum,"sine1",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes
	}
	//sine2
	//sine3
	
	// close a file, write header, after DiskOut usage
	close { arg completionMessage;
		server.listSendMsg( this.closeMsg(completionMessage)  );
	}
	closeMsg { arg completionMessage;
		^["/b_close", bufnum, completionMessage.value(this) ];
	}
	
	//private	
	alloc { arg completionMessage;
		server.listSendMsg( this.allocMsg(completionMessage) )
	}
	allocRead { arg argpath,startFrame,completionMessage;
		path = argpath;
		server.listSendMsg(this.allocReadMsg( argpath,startFrame,completionMessage));
	}
	
	*loadDialog { arg server,startFrame = 0,numFrames = -1;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames).loadDialog(startFrame)
	}
	loadDialog { arg startFrame, completionMessage;
		File.openDialog("Select a file...",{ arg path;
			this.allocRead(path, startFrame, completionMessage);
		});
	}
}

