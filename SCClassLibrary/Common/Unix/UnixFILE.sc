UnixFILE : IOStream {
	// abstract class. Use File or Pipe.

	classvar <openFiles;
	var fileptr;
	// this only supports file sizes < 2^31 for now

	*closeAll {
		if(openFiles.notNil) {
			openFiles.copy.do { |file| file.close }
		};
		openFiles = nil
	}
	isOpen {
		^fileptr.notNil
	}

	next { ^this.getChar }
	nextN { |n|
		^String.fill(n, { this.next })
	}
	contents {
		^this.readAllString
	}

	put { |item| this.write(item) }
	putAll { |aCollection|
		if(aCollection.isKindOf(RawArray)) {
			this.write(aCollection)
		} {
			aCollection.do { |item| this.write(item) }
		}
	}

	flush {
		_FileFlush
		^this.primitiveFailed
	}

	write { |item|
		/* writes any of the following items in big endian byte order:
			a double float,
			a 32 bit int,
			a char,
			the name of a Symbol as chars,
			RawArrays,
				(i.e. Strings, Int8Arrays, Int16Arrays,
				Signals, etc.)
		*/
		_FileWrite
		^this.primitiveFailed
	}
	read { |buffer|
		// reads big endian data
		// buffer should be a RawArray.
		// fills the buffer, or as much is available.
		// returns bytes read.
		_FileReadRaw;
		^this.primitiveFailed
	}

	writeLE { |item|
		/* writes any of the following items in little endian byte order:
			a double float,
			a 32 bit int,
			a char,
			the name of a Symbol as chars,
			RawArrays,
				(i.e. Strings, Int8Arrays, Int16Arrays,
				Signals, etc.)
		*/
		_FileWriteLE
		^this.primitiveFailed
	}
	readLE { |buffer|
		// reads little endian data
		// buffer should be a RawArray.
		// fills the buffer, or as much is available.
		// returns bytes read.
		_FileReadRawLE;
		^this.primitiveFailed
	}

	getLine { |maxSize = 1024|
		var string;
		string = String.newClear(maxSize);
		^this.prGetLine(string)
	}
	prGetLine { |argString|
		// returns a string up to lesser of next newline
		// or length-1 of the argument string
		_FileReadLine;
		^this.primitiveFailed
	}

	// for more fine grained control these read and write a single
	// item of the specified type and size
	getChar { _FileGetChar; ^this.primitiveFailed }
	getInt8 { _FileGetInt8; ^this.primitiveFailed }
	getInt16 { _FileGetInt16; ^this.primitiveFailed }
	getInt32 { _FileGetInt32; ^this.primitiveFailed }
	getFloat { _FileGetFloat; ^this.primitiveFailed }
	getDouble { _FileGetDouble; ^this.primitiveFailed }
	getInt16LE { _FileGetInt16LE; ^this.primitiveFailed }
	getInt32LE { _FileGetInt32LE; ^this.primitiveFailed }
	getFloatLE { _FileGetFloatLE; ^this.primitiveFailed }
	getDoubleLE { _FileGetDoubleLE; ^this.primitiveFailed }

	putChar { |aChar| _FilePutChar; ^this.primitiveFailed }
	putInt8 { |anInteger| _FilePutInt8; ^this.primitiveFailed }
	putInt16 { |anInteger| _FilePutInt16; ^this.primitiveFailed }
	putInt32 { |anInteger| _FilePutInt32; ^this.primitiveFailed }
	putFloat { |aFloat| _FilePutFloat; ^this.primitiveFailed }
	putDouble { |aFloat| _FilePutDouble; ^this.primitiveFailed }
	putInt16LE { |anInteger| _FilePutInt16LE; ^this.primitiveFailed }
	putInt32LE { |anInteger| _FilePutInt32LE; ^this.primitiveFailed }
	putFloatLE { |aFloat| _FilePutFloatLE; ^this.primitiveFailed }
	putDoubleLE { |aFloat| _FilePutDoubleLE; ^this.primitiveFailed }
	putString { |aString| _FilePutString; ^this.primitiveFailed }
	putString0 { |aString|
		this.putString(aString);
		this.putInt8(0)
	}

	putPascalString { |aString|
		if(aString.size >= 256) {
			Error("putPascalString: input string must be shorter than 256 chars: \"%\"".format(aString)).throw
		};
		this.putInt8(aString.size);
		this.putString(aString)
	}

	getPascalString {
		var size, string;
		size = this.getInt8;
		string = String.newClear(size);
		this.read(string);
		^string
	}

	// PRIVATE
	addOpenFile {
		openFiles = openFiles.add(this)
	}
}
