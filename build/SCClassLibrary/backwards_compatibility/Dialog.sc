
GetFileDialog {

	*new { arg function;
		CocoaDialog.getPaths({arg paths; function.value(paths.first) },nil)
	}

}