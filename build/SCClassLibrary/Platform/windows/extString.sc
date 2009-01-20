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
}