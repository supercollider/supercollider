
GetFileDialog {

	*new { arg function;
		// platform neutral (eventually)
		File.openDialog(nil,
			{ arg path; function.value(true,path) },
			{  function.value(false) } );
		//CocoaDialog.getPaths({arg paths; function.value(paths.first) },nil)
	}
	// warning:  doesn't handle doAction (no instance)

}