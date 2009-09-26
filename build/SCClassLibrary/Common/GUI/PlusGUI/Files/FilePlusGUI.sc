+ File {
	*openDialog { arg prompt, successFunc, cancelFunc;
		var gui;
		gui = GUI.current;
		gui.dialog.getPaths(
			{ arg paths; GUI.use( gui, { successFunc.value( paths.first )})},
			{ if(cancelFunc.notNil){ GUI.use( gui, cancelFunc )}}, 3 );

		// 	var path;
		//	path = this.prOpenDialog(prompt);
		//	if (path.notNil, { successFunc.value(path) },{ cancelFunc.value(path) });
	}

	*saveDialog { arg prompt, defaultName, successFunc, cancelFunc;
		var gui;
		gui = GUI.current;
		// no prompts or defaultNames in cocoa
		gui.dialog.savePanel({ arg args;
			GUI.use( gui, { successFunc.value( args )})}, {
			if(cancelFunc.notNil){GUI.use( gui, cancelFunc )}}
		);
	}
}
