
Buffer {

	var <server, <bufnum, <>numFrames, <>numChannels = 1;

	*new { arg server,numFrames,numChannels=1;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames,
						numChannels).alloc
	}	
	// read whole file into memory
	*read { arg server,path,startFrame = 0,numFrames = -1;
		server = server ? Server.local;
		^super.newCopyArgs(server,
						server.bufferAllocator.alloc(1),
						numFrames).allocRead(path,startFrame)
	}
	// preload a buffer for use with DiskIn
	*cueSoundFile { arg server,path,startFrame = 0,numChannels= 2, bufferSize = 32768;
		^this.new(server,bufferSize,numChannels)
			.read(path,startFrame,-1,0,true)
	}
	
	read { arg path, fileStartFrame = 0, numFrames = -1, 
					bufStartFrame = 0, leaveOpen = false;
		this.numFrames = numFrames;
		server.sendMsg("/b_read", bufnum, path, 
				fileStartFrame, numFrames, 
				bufStartFrame, leaveOpen.binaryValue);
		// check size and channels if don't know
	}
	write { arg path,headerFormat="aiff",sampleFormat="int24",numFrames = -1,
						startFrame = 0,leaveOpen = false;
		server.sendMsg("/b_write", bufnum, path, 
				headerFormat,sampleFormat, numFrames, startFrame, 
				leaveOpen.binaryValue);
		// writeEnabled = true;
	}
	free {
		server.sendMsg("/b_free", bufnum);
		server.bufferAllocator.free(bufnum);
	}
	
	zero {
		server.sendMsg("/b_zero", bufnum);
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
	
	// close a file, write header, after DiskOut usage
	close {
		server.sendMsg("/b_close", bufnum);
	}
	
	//private	
	alloc {
		server.sendMsg("/b_alloc", bufnum, numFrames, numChannels);
	}
	allocRead { arg path,startFrame;
		server.sendMsg("/b_allocRead",bufnum, path,startFrame,numFrames);
		// check size and channels if don't know
	}
}
