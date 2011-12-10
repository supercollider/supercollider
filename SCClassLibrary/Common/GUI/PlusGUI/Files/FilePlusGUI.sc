+ File {
	*openDialog { arg prompt, successFunc, cancelFunc;
		var gui = GUI.current,
			dialogClass = GUI.dialog,
			dialogMetaClass;

		/*
			Older versions of SwingOSC have an older argument, maxSize,
			that disagrees with CocoaDialog and QDialog. Newer versions
			of SwingOSC have the same arguments. So it isn't enough to
			check GUI.id -- we have to detect which argument name is there.

			Then, findRespondingMethodFor searches a class's methods.
			But getPaths is a class method -- of the metaclass -- and,
			Class provides no easy way to navigate from a class
			to its metaclass.

			So it's rather more complex than I suppose it really needs to be.
			Backward compatibility is not always pain-free.
		*/

		dialogMetaClass = ("Meta_" ++ dialogClass.name).asSymbol.asClass;

		if(dialogMetaClass.findRespondingMethodFor(\getPaths).argNames.includes(\maxSize)) {
			dialogClass.getPaths(
				{ arg paths; GUI.use( gui, { successFunc.value( paths.first )})},
				{ if(cancelFunc.notNil){ GUI.use( gui, cancelFunc )}}, 3 );
		} {
			dialogClass.getPaths(
				{ arg paths; GUI.use( gui, { successFunc.value( paths.first )})},
				{ if(cancelFunc.notNil){ GUI.use( gui, cancelFunc )}}, true );
		};
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
