+ File {
	*openDialog { arg prompt, successFunc, cancelFunc;
		// replace with platform independant stuff later
		CocoaDialog.getPaths({arg paths; successFunc.value(paths.first) },cancelFunc,3);

		// 	var path;
		//	path = this.prOpenDialog(prompt);
		//	if (path.notNil, { successFunc.value(path) },{ cancelFunc.value(path) });
	}
	
	*saveDialog { arg prompt, defaultName, successFunc, cancelFunc;
		// no prompts or defaultNames in cocoa
		CocoaDialog.savePanel(successFunc,cancelFunc);

		//	var path;
		//	path = this.prSaveDialog(prompt, defaultName);
		//	if (path.notNil, { successFunc.value(path) },{ cancelFunc.value(path) });
	}
	
	// returns a string for the file's full path.
	// returns nil if canceled.
	*prOpenDialog { arg prompt;
		_File_GetFile
		^this.primitiveFailed;
	}
	// returns a string for the file's full path.
	// returns nil if canceled.
	*prSaveDialog { arg prompt, defaultName;
		_File_PutFile
		^this.primitiveFailed;
	}
}
