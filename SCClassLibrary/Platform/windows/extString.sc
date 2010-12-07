+ String {
/*
		// used in methodReferences lookup
	newTextWindow { arg title="Untitled", makeListener=false;
		var tempFile, path;
	//	this.postln;
		path = "temp_newTextWindow";
		tempFile = File(path, "w");
		tempFile.write(this);
		tempFile.close;
		^path.openWinTextFile;
	}
*/
	openTextFile { arg selectionStart=0, selectionLength=0;
		^this.openWinTextFile(selectionStart, selectionLength);
	}

	hackOpenWinTextFile { arg path, rangeStart, rangeSize;
		_OpenWinTextFile ;//doc = Document.open(PathName(this).asAbsolutePath , selectionStart, selectionLength);
	}

	openWinTextFile{ arg selectionStart=0, selectionLength=0;
		//hackOpenWinTextFile(PathName(this).asAbsolutePath , selectionStart, selectionLength); // standardizePath on win32 yet
		hackOpenWinTextFile(this, selectionStart, selectionLength);
	}

		// windows expresses absolute paths differently from posix
	absolutePath {
		var first, sep;
		sep = thisProcess.platform.pathSeparator;
			// 2 styles of absolute path:
			// C:\... or \\machine_name\...
		if((this[0].isAlpha and: { this[1] == $: and: { this[2] == sep } })
				or: { this[0] == sep and: { this[1] == sep } }) {
			^this
		} {
				// let's also try to convert unix-style dirs
				// assume starting from root dir of drive letter where SC is installed
			if(this[0] == $/ and: { this[1] != $/ }) {
				^File.getcwd[..1] ++ this
			};
				// currently we can't support this on windows
				// though it would be nice to have a primitive to get the user's home dir
//			if(first == $~){^this.standardizePath};
			^File.getcwd ++ sep ++ this;
		}
	}
}


+ PathName {
	isAbsolutePath {
		var	sep = thisProcess.platform.pathSeparator;
		^(fullPath[0].isAlpha and: { fullPath[1] == $: and: { fullPath[2] == sep } })
				or: { fullPath[0] == sep and: { fullPath[1] == sep } }
				or: { fullPath[0] == $/ and: { fullPath[1] != $/ } }
	}
}
