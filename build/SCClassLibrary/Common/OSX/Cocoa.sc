
CocoaDialog {

	classvar openDialogs;
	
	var returnSlot;
	
	*getPaths { arg okFunc,cancelFunc,
				maxSize=20;
		var new,returnSlot;
		returnSlot = Array.new(maxSize);
		new = super.newCopyArgs(returnSlot);
		new.retainDialog;
		{
			new.prGetPathsDialog(returnSlot);
			if(returnSlot.size > 0,{
				okFunc.value(returnSlot);
			},{
				cancelFunc.value(returnSlot);
			});
			new.releaseDialog;
			nil
		}.defer;
		^new
	}

	prGetPathsDialog { arg returnSlot;
		_Cocoa_GetPathsDialog
		^this.primitiveFailed
	}
		
	retainDialog {
		openDialogs = openDialogs.add(this);
	}
	releaseDialog {
		openDialogs.remove(this);
	}

}


/*
Cocoa

	*saveAsPlist { arg object,path;
		_Cocoa_SaveAsPlist
		^this.primitiveFailed
	}

*/

