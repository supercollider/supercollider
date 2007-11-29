+ String { 
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
	openTextFile { arg selectionStart=0, selectionLength=0;
		^this.openWinTextFile(selectionStart, selectionLength);
	}
		// make pathmatch on win conform to unix behavior.
	pathMatch { var res;
		res = this.prPathMatch;
		^if (res.isKindOf(String), [], res);
	}

	prPathMatch { _StringPathMatch ^this.primitiveFailed } // orig
}
