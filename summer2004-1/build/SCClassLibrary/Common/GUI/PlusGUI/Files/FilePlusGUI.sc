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
}
