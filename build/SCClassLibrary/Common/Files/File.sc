
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
	length { // returns the length of the file
		_FileLength;
		^this.primitiveFailed;
	}		
	pos { // current file position
		_FilePos 
		^this.primitiveFailed;
	}
	seek { arg offset, origin; 
		// origin is an integer, one of: 
		// 0 - from beginning of file
		// 1 - from current position
		// 2 - from end of file
		
		_FileSeek
		^this.primitiveFailed;
	}
	pos_ { arg toPos;
		toPos = toPos.clip(0, this.length - 1);
		this.seek(toPos, 0);
	}
	
	next { ^this.getChar }
	nextN { arg n;
		^String.fill(n, { this.next; });
	}
	contents {
		^this.read(String.new(this.length));
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
	readAllString {
		var string;
		string = String.newClear(this.length);
		this.read(string);
		^string
	}
	
	getLine { arg maxSize=1024;
		var string;
		string = String.newClear(maxSize);
		this.prGetLine(string);
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

File : UnixFILE {
	
	classvar <openDialogs;
	
	*openDialog { arg prompt, successFunc, cancelFunc;
		var path;
		// replace with platform independant stuff later
		CocoaDialog.getPaths({arg paths; successFunc.value(paths.first) },cancelFunc,3);
		//		path = this.prOpenDialog(prompt);
		//		if (path.notNil, { successFunc.value(path) },{ cancelFunc.value(path) });
	}
	
//  not yet implemented
//	*saveDialog { arg prompt, defaultName, successFunc, cancelFunc;
//		var path;
//		path = this.prSaveDialog(prompt, defaultName);
//		if (path.notNil, { successFunc.value(path) },{ cancelFunc.value(path) });
//	}
	
	*new { arg pathName, mode; 
		^super.new.open(pathName, mode);
	}
	*delete { arg pathName;
		_FileDelete
		^this.primitiveFailed
	}
	*exists { arg pathName;
		var file;
		file = File(pathName,"r");
		if (file.isOpen, { file.close; ^true });
		^false
	}
	open { arg pathName, mode;
		/* open the file. mode is a string passed
			to fopen, so should be one of:
			"r","w","a","rb","wb","ab","r+","w+","a+",
			"rb+","wb+","ab+" 
		*/
		if (this.prOpen(pathName, mode), {
			this.addOpenFile;
		});
	}
	close {	// close the file
		// the GC will not call this for you
		this.prClose;
		openFiles.remove(this);
	}
	prClose {
		_FileClose 
		^this.primitiveFailed;
	}
	
	// PRIVATE
	prOpen { arg pathName, mode;
		/* open the file. mode is a string passed
			to fopen, so should be one of:
			"r","w","a","rb","wb","ab","r+","w+","a+",
			"rb+","wb+","ab+" 
		*/
		_FileOpen 
		^this.primitiveFailed;
	}

	// returns a string for the file's full path.
	// returns nil if canceled.
	*prOpenDialog { arg prompt;
		_File_GetFile
		^this.primitiveFailed;
	}
	// returns a string for the file's full path.
	// returns nil if canceled.
	*prSaveDialog { arg prompt, defaultName;
		_File_PutFile
		^this.primitiveFailed;
	}
	
}


Pipe : UnixFILE {
	// pipe stdin to, or stdout from, a unix shell command.
	*new { arg commandLine, mode; 
		^super.new.open(commandLine, mode);
	}
	open { arg commandLine, mode;
		/* open the file. mode is a string passed
			to popen, so should be one of:
			"r","w" 
		*/
		if (this.prOpen(commandLine, mode), {
			this.addOpenFile;
		});
	}
	
	close { // close the file
		// the GC will not call this for you
		_PipeClose 
		^this.primitiveFailed;
	}

	
	// PRIVATE
	prOpen { arg pathName, mode;
		/* open the file. mode is a string passed
			to popen, so should be one of:
			"r","w" 
		*/
		_PipeOpen 
		^this.primitiveFailed;
	}
}


