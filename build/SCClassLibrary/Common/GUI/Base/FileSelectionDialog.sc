FileSelectionDialog {

	classvar result, ok, cancel;
	
	*getPaths { arg okFunc, cancelFunc, maxSize=20;
		if(result.notNil,{
			"A CocoaDialog is already in progress.  do: [CocoaDialog.clear]".warn;
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
	*savePanel { arg okFunc,cancelFunc;
		if(result.notNil,{
			"A CocoaDialog is already in progress.  do: [CocoaDialog.clear]".warn;
			^nil
		});
		result = String.new(512);
		ok = okFunc;
		cancel = cancelFunc;
		this.prSavePanel(result);
	}
	*prSavePanel { arg argResult;
		_Cocoa_SavePanel
		^this.primitiveFailed
	}
			
	*ok {
		var res;
		res = result;
		cancel = result = nil;
		ok.value(res);
		ok = nil;
	}
	*cancel {
		var res;
		res = result;
		ok = result = nil;
		cancel.value(res);
		cancel = nil;
	}
	*error {
		this.clear;
		"An error has occured during a CocoaDialog".error;
	}
	*clear { // in case of errors, invalidate any previous dialogs
		ok = cancel = result = nil;
	}
}
