
CocoaDialog {

	classvar returnSlot,ok,cancel;
	
	*getPaths { arg okFunc,cancelFunc;
		if(returnSlot.isNil,{
			returnSlot = Array.new;
			ok = okFunc;
			cancel = cancelFunc;
			this.prGetPathsDialog(returnSlot);
		},{
			"A CocoaDialog is already in progress.".warn;
		});
	}
	
	*prGetPathsDialog { arg returnSlot;
		_Cocoa_GetPathsDialog
		^this.primitiveFailed
	}

	*ok {
		ok.value(returnSlot);
		ok = cancel = returnSlot = nil;
	}
	*cancel {
		cancel.value;
		ok = cancel = returnSlot = nil;
	}
	*clear { // in case of errors, invalidate any previous dialogs
		returnSlot = nil;
	}
}

