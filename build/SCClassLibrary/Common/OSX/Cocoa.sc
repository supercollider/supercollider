
CocoaDialog {

	classvar result, ok, cancel;
	
	*getPaths { arg okFunc, cancelFunc, maxSize=20;
		if(result.notNil,{
			"A CocoaDialog is already in progress.".warn;
			^nil
		});
		
		result = Array.new(maxSize);
		ok = okFunc;
		cancel = cancelFunc;
		this.prGetPathsDialog(result);
	}
	
	*prGetPathsDialog { arg argResult;
		_Cocoa_GetPathsDialog
		^this.primitiveFailed
	}

	*ok {
		ok.value(result);
		ok = cancel = result = nil;
	}
	*cancel {
		cancel.value;
		ok = cancel = result = nil;
	}
	*clear { // in case of errors, invalidate any previous dialogs
		result = nil;
	}
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

