File : UnixFILE {

	classvar <systemIsCaseSensitive;

	*initClass {
		var f = this.filenameSymbol.asString;
		systemIsCaseSensitive = not(File.exists(f.toLower) and: {File.exists(f.toUpper)});
	}

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
	*readAllString { arg pathName;
		var string;
		this.use(pathName, "r", { |file| string = file.readAllString });
		^string
	}
	*readAllSignal { arg pathName;
		var string;
		this.use(pathName, "r", { |file| string = file.readAllSignal });
		^string
	}
	*readAllStringHTML { arg pathName;
		var string;
		this.use(pathName, "r", { |file| string = file.readAllStringHTML });
		^string
	}
	*readAllStringRTF { arg pathName;
		var string;
		this.use(pathName, "r", { |file| string = file.readAllStringRTF });
		^string
	}
	*delete { arg pathName;
		_FileDelete
		^this.primitiveFailed
	}
	*deleteAll { arg pathName;
		_FileDeleteAll
		^this.primitiveFailed
	}
	*mtime { arg pathName;
		_FileMTime
		^this.primitiveFailed
	}
	*exists { arg pathName;
		_FileExists
		^this.primitiveFailed
	}
	*existsCaseSensitive { arg pathName;
		^if(systemIsCaseSensitive) {
			this.exists(pathName)
		} {
			(pathName.dirname +/+ "*").pathMatch
				.detect({ |x| x.withoutTrailingSlash.compare(pathName.withoutTrailingSlash) == 0 })
				.notNil
		}
	}
	*realpath { arg pathName;
		_FileRealPath
		^this.primitiveFailed
	}
	*mkdir { arg pathName;
		_FileMkDir
		^this.primitiveFailed
	}
	*copy { arg pathNameFrom, pathNameTo;
		_FileCopy
		^this.primitiveFailed
	}
	*type { arg pathName;
		var sym = #[error, not_found, regular, directory, symlink, block, character, fifo, socket, unknown];
		^sym.clipAt(this.prType(pathName))
	}
	*prType { arg pathName;
		_FileType
		^this.primitiveFailed
	}
	*fileSize { arg pathName;
		_FileSize
		^this.primitiveFailed
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
	var pid;

	// pipe stdin to, or stdout from, a unix shell command.
	*new { arg commandLine, mode;
		^super.new.open(commandLine, mode)
	}

	*argv { arg args, mode;
		^super.new.openArgv(args, mode);
	}

	*call { arg command, onSuccess, onError, maxLineLength=4096;
		var r, cancel, closePipe;
		r = Routine.run({
			{
				closePipe = Pipe.callSync(command, onSuccess, onError, maxLineLength)
			}.protect({
				CmdPeriod.remove(cancel);
			});
		}, clock: AppClock);
		cancel = {
			r.stop();
			("Closed:" + command).postln;
			closePipe.value();
			CmdPeriod.remove(cancel);
		};
		CmdPeriod.add(cancel);
		^cancel
	}
	*callSync { arg command, onSuccess, onError, maxLineLength=4096;
		var pipe, lines=[], line, close, exitCode;
		pipe = Pipe.new(command, "r");
		close = {
			var exitCode;
			if(pipe.isOpen, {
				exitCode = pipe.close();
			});
			CmdPeriod.remove(close);
			close = nil;
			// this function might be callSync's return.
			// The user might call it independently.
			// So, return the exit code here.
			exitCode
		};
		CmdPeriod.add(close);
		line = pipe.getLine(maxLineLength);
		if(line.isNil, {
			exitCode = close.value();
			onError.value(exitCode);
		}, {
			while({ line.notNil }, {
				lines = lines.add(line);
				line = pipe.getLine(maxLineLength);
			});
			exitCode = close.value();
			onSuccess.value(lines.join(Char.nl), exitCode);
		});
		^close
	}

	open { arg commandLine, mode;
		/* open the file. mode is a string passed
			to popen, so should be one of:
			"r","w"
		*/
		if ((pid = this.prOpen(commandLine, mode)).notNil) {
			this.addOpenFile;
		}
	}

	openArgv { arg args, mode;
		/* open the file. mode is a string passed
			to popen, so should be one of:
			"r","w"
		*/
		if ((pid = this.prOpenArgv(args, mode)).notNil) {
			this.addOpenFile;
		}
	}

	close {
		var res = this.prClose(pid);
		fileptr = nil;
		pid = nil;
		openFiles.remove(this);
		^res
	}

	// close the file
	prClose { arg pid;
		// the GC will not call this for you
		_PipeClose
		^this.primitiveFailed
	}


	// PRIVATE
	prOpen { arg pathName, mode;
		/* open the file. mode is a string passed
			to popen, so should be one of:
			"r","w"
		*/
		_PipeOpen
		^this.primitiveFailed
	}

	prOpenArgv { arg  args, mode;
		/* open the file with explicit list of arguments. mode is a string passed
			to popen, so should be one of:
			"r","w"
		*/
		_PipeOpenArgv
		^this.primitiveFailed
	}
}
