UnixFILE : IOStream {
	// abstract class. Use File or Pipe.
	
	classvar <openFiles;
	var fileptr;
	// this only supports file sizes < 2^31 for now

	*closeAll {
		if (openFiles.notNil, {
			openFiles.copy.do({ arg file; file.close; });
		});
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

	write { arg item; 
		/* writes any of the following items:
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
		// buffer should be a RawArray.
		// fills the buffer, or as much is available.
		// returns bytes read.
		_FileReadRaw;
		^this.primitiveFailed;
	}
	
	getLine { arg maxSize=1024;
		var string;
		string = String.newClear(maxSize);
		string = this.prGetLine(string);
		if (string.isNil, { ^nil });
		if(string.at(string.size - 1) == $\n,{
			^string.copyRange(0,string.size - 2)
		},{
			^string.copy		
		});
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
	
	putChar { arg aChar; _FilePutChar; ^this.primitiveFailed; }
	putInt8 { arg anInteger; _FilePutInt8; ^this.primitiveFailed; }
	putInt16 { arg anInteger; _FilePutInt16; ^this.primitiveFailed; }
	putInt32 { arg anInteger; _FilePutInt32; ^this.primitiveFailed; }
	putFloat { arg aFloat; _FilePutFloat; ^this.primitiveFailed; }
	putDouble { arg aFloat; _FilePutDouble; ^this.primitiveFailed; }
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
		var le;
		le = this.getInt8;
		^this.nextN(le);
	}
	// PRIVATE
	addOpenFile {
		openFiles = openFiles.add(this);
	}
}
