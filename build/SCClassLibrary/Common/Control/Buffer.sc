
Buffer {

	var <server, <bufnum, <>numFrames, <>numChannels = 1;
	var <path;
	
	*new { arg server,numFrames,numChannels=1,completionMessage;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames,
						numChannels).alloc(completionMessage)
	}	
	// read whole file into memory
	*read { arg server,path,startFrame = 0,numFrames = -1, completionMessage;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames).allocRead(path,startFrame,completionMessage)
	}
	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server,path,startFrame = 0,numChannels= 2, bufferSize = 32768,completionMessage;
		^this.new(server,bufferSize,numChannels)
			.read(path,startFrame,-1,0,true,completionMessage)
	}
	
	read { arg argpath, fileStartFrame = 0, numFrames = -1, 
					bufStartFrame = 0, leaveOpen = false, completionMessage;
		this.numFrames = numFrames;
		path = argpath;
		server.sendMsg("/b_read", bufnum, path, 
				fileStartFrame, numFrames, 
				bufStartFrame, leaveOpen.binaryValue, completionMessage.value(this) ?? {[]});
		// check size and channels if don't know ?
	}
	write { arg path,headerFormat="aiff",sampleFormat="int24",numFrames = -1,
						startFrame = 0,leaveOpen = false, completionMessage;
		// doesn't change my path 
		server.sendMsg("/b_write", bufnum, path, 
				headerFormat,sampleFormat, numFrames, startFrame, 
				leaveOpen.binaryValue, completionMessage.value(this) ?? {[]});
		// writeEnabled = true;
	}
	free { arg completionMessage;
		server.sendMsg("/b_free", bufnum, completionMessage.value(this) ?? {[]});
		server.bufferAllocator.free(bufnum);
	}
	
	zero { arg completionMessage;
		server.sendMsg("/b_zero", bufnum, completionMessage.value(this) ?? {[]});
	}
	set { arg index,float ... morePairs;
		server.performList(\sendMsg,["/b_set",bufnum,index,float] 
			++ morePairs);
	}
	setn { arg startAt , values ... morePairs;
		server.performList(\sendMsg,["/b_setn",bufnum,startAt,values.size] 
			++ values ++ morePairs.flat);
	}
	fill { arg startAt,numFrames,value ... more;
		server.performList(\sendMsg,["/b_fill",bufnum,startAt,numFrames,value]
			 ++ more);
	}
	sine1 { arg amplitudes,normalize=false,asWavetable=false,clearFirst=false;
		server.performList(\sendMsg,["/b_gen",bufnum,"sine1",
			normalize.binaryValue 
			+ (asWavetable.binaryValue * 2) 
			+ (clearFirst.binaryValue * 4)]
			++ amplitudes)
	}
	//sine2
	//sine3
	
	// close a file, write header, after DiskOut usage
	close { arg completionMessage;
		server.sendMsg("/b_close", bufnum, completionMessage.value(this)??{[]});
	}
	
	//private	
	alloc { arg completionMessage;
		server.sendMsg("/b_alloc", bufnum, numFrames, numChannels, completionMessage.value(this) ?? {[]});
	}
	allocRead { arg argpath,startFrame,completionMessage;
		path = argpath;
		server.sendMsg("/b_allocRead",bufnum, path,startFrame,numFrames,completionMessage.value(this) ?? {[]});
		// check size and channels if don't know
	}
	
	// convienient
	*loadDialog { arg server,startFrame = 0,numFrames = -1;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames).loadDialog(startFrame)
	}

	loadDialog { arg startFrame, completionMessage;
		CocoaDialog.getPaths({ arg paths;
			path = paths.at(0);
			this.allocRead(path, startFrame, completionMessage);
		});
	}	

}
