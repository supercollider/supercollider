/*
	Sound File Format symbols:
		header formats:
			read/write formats:
				'AIFF', 	- Apple's AIFF
				'WAV','RIFF' 	- Microsoft .WAV
				'SD2', 	- Sound Designer 2
				'Sun', 	- NeXT/Sun
				'IRCAM', 	- old IRCAM format
				'none'	- no header = raw data
			A huge number of other formats are supported read only.
			
		sample formats:
			'int8', 'int16', 'int24', 'int32'
			'mulaw', 'alaw',
			'float32'
			
		not all header formats support all sample formats.

*/

SoundFile {	
	classvar <openFiles;
	
	var <>fileptr;
	var <>headerFormat = 'aiff';
	var <>sampleFormat = 'float32';
	var <numFrames = 0;		// number of frames
	var <>numChannels = 1;	// number of channels
	var <>sampleRate = 44100.0;

	
	*closeAll {
		if (openFiles.notNil, {
			openFiles.copy.do({ arg file; file.close; });
		});
	}
	isOpen {
		^fileptr.notNil
	}

	openRead { arg pathName; 
		// returns true if success, false if file not found or error reading.
		_SFOpenRead
		^this.primitiveFailed;
	}
	readData { arg rawArray;
		// must have called openRead first!
		// returns true if success, false if file not found or error reading.
		_SFRead
		^this.primitiveFailed;
	}
	
	readHeaderAsString {
		// must have called openRead first!
		//returns the whole header as String
		_SFHeaderInfoString
		^this.primitiveFailed;

	}
	openWrite { arg pathName;
		// write the header
		// format written is that indicated in headerFormat and sampleFormat. 
		// return true if successful, false if not found or error writing.
		_SFOpenWrite
		^this.primitiveFailed;
	}
	writeData { arg rawArray;
		// must have called openWrite first!
		// format written is that indicated in sampleFormat. 
		// return true if successful, false if not found or error writing.
		_SFWrite
		^this.primitiveFailed;
	}
	close {
		_SFClose
		^this.primitiveFailed;
	}

	seek { arg offset, origin; 
		// origin is an integer, one of: 
		// 0 - from beginning of file
		// 1 - from current position
		// 2 - from end of file
		_SFSeek
		^this.primitiveFailed;
	}
	
	duration { ^numFrames/sampleRate }
}
