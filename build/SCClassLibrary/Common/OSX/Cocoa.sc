CocoaDialog : FileSelectionDialog {
}

Cocoa {

	*getPathsInDirectory { arg directoryPath,extension,maxItems=1000;
		^this.prGetPathsInDirectory(directoryPath,extension,Array.new(maxItems));
		//throws an index out of range if more than maxItems items are in the directory
		
		//extension matching not yet implemented
	}
	*prGetPathsInDirectory { arg dir,ext,arr;
		_Cocoa_GetPathsInDirectory;
		^this.primitiveFailed
	}
}
