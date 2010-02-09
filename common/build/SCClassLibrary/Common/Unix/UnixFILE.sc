UnixFILE : IOStream {
	// abstract class. Use File or Pipe.

	classvar <openFiles;
	var fileptr;
	// this only supports file sizes < 2^31 for now

	*closeAll {
		if (openFiles.notNil, {
			openFiles.copy.do({ arg file; file.close; });
		});
		openFiles = nil;
	}
	isOpen {
		^fileptr.notNil
	}

	next { ^this.getChar }
	nextN { arg n;
		^String.fill(n, { this.next; });
	}
	contents {
		^this.readAllString;
	}

	put { arg item; this.write(item); }
	putAll { arg aCollection;
		if (aCollection.isKindOf(RawArray), {
			this.write( aCollection );
		},{
			aCollection.do({ arg item; this.write(item) });
		});
	}

	flush {
		_FileFlush
		^this.primitiveFailed
	}

	write { arg item;
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
		^this.primitiveFailed;
	}
	read { arg buffer;
		// reads big endian data
		// buffer should be a RawArray.
		// fills the buffer, or as much is available.
		// returns bytes read.
		_FileReadRaw;
		^this.primitiveFailed;
	}

	writeLE { arg item;
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
		^this.primitiveFailed;
	}
	readLE { arg buffer;
		// reads little endian data
		// buffer should be a RawArray.
		// fills the buffer, or as much is available.
		// returns bytes read.
		_FileReadRawLE;
		^this.primitiveFailed;
	}

	getLine { arg maxSize=1024;
		var string;
		string = String.newClear(maxSize);
		^this.prGetLine(string);
	}
	prGetLine { arg argString;
		// returns a string up to lesser of next newline
		// or length-1 of the argument string
		_FileReadLine;
		^this.primitiveFailed;
	}

	// for more fine grained control these read and write a single
	// item of the specified type and size
	getChar { _FileGetChar; ^this.primitiveFailed; }
	getInt8 { _FileGetInt8; ^this.primitiveFailed; }
	getInt16 { _FileGetInt16; ^this.primitiveFailed; }
	getInt32 { _FileGetInt32; ^this.primitiveFailed; }
	getFloat { _FileGetFloat; ^this.primitiveFailed; }
	getDouble { _FileGetDouble; ^this.primitiveFailed; }
	getInt16LE { _FileGetInt16LE; ^this.primitiveFailed; }
	getInt32LE { _FileGetInt32LE; ^this.primitiveFailed; }
	getFloatLE { _FileGetFloatLE; ^this.primitiveFailed; }
	getDoubleLE { _FileGetDoubleLE; ^this.primitiveFailed; }

	putChar { arg aChar; _FilePutChar; ^this.primitiveFailed; }
	putInt8 { arg anInteger; _FilePutInt8; ^this.primitiveFailed; }
	putInt16 { arg anInteger; _FilePutInt16; ^this.primitiveFailed; }
	putInt32 { arg anInteger; _FilePutInt32; ^this.primitiveFailed; }
	putFloat { arg aFloat; _FilePutFloat; ^this.primitiveFailed; }
	putDouble { arg aFloat; _FilePutDouble; ^this.primitiveFailed; }
	putInt16LE { arg anInteger; _FilePutInt16LE; ^this.primitiveFailed; }
	putInt32LE { arg anInteger; _FilePutInt32LE; ^this.primitiveFailed; }
	putFloatLE { arg aFloat; _FilePutFloatLE; ^this.primitiveFailed; }
	putDoubleLE { arg aFloat; _FilePutDoubleLE; ^this.primitiveFailed; }
	putString { arg aString; _FilePutString; ^this.primitiveFailed; }
	putString0 { arg aString;
		this.putString(aString);
		this.putInt8(0);
	}
	putPascalString { arg aString;
		this.putInt8(aString.size);
		this.putString(aString);
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
		openFiles = openFiles.add(this);
	}
}
