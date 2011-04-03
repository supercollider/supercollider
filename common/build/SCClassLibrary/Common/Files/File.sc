File : UnixFILE {

	classvar <openDialogs;

	*new { arg pathName, mode;
		^super.new.open(pathName, mode);
	}
	*open { arg pathName, mode;
		^super.new.open(pathName, mode);
	}
	*use { arg pathName, mode, function;
		var file;
		file = this.new(pathName, mode);
		^{ function.value(file) }.protect({ file.close });
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
	*getcwd {
		var string;
		this.prGetcwd(string = String.new(256));
		^string
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
		fileptr = nil;
		openFiles.remove(this);
	}
	length { // returns the length of the file
		_FileLength;
		^this.primitiveFailed;
	}
	pos { // current file position
		_FilePos
		^this.primitiveFailed;
	}
	pos_ { arg toPos;
		toPos = toPos.clip(0, this.length - 1);
		this.seek(toPos, 0);
	}
	seek { arg offset = 0, origin = 0;
		// origin is an integer, one of:
		// 0 - from beginning of file
		// 1 - from current position
		// 2 - from end of file

		_FileSeek
		^this.primitiveFailed;
	}

	readAllString {
		var string;
		string = String.newClear(this.length);
		this.read(string);
		^string
	}
	readAllStringRTF {
		^this.readAllString.stripRTF;
	}
	readAllStringHTML {
		^this.readAllString.stripHTML;
	}
	readAllSignal {
		var signal;
		signal = Signal.newClear(this.length);
		this.read(signal);
		^signal
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
	prClose {
		_FileClose
		^this.primitiveFailed;
	}

	*prGetcwd { arg string;
		_File_getcwd
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

	close {
		this.prClose;
		fileptr = nil;
		openFiles.remove(this);
	}

	prClose { // close the file
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


