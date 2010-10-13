
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


GetStringDialog  {

	*new { arg prompt,defaultString="",func;
		var b;
		Sheet({ arg l;
			b = 	TextField(l,Rect(0,0,150,30));
			b.string = String.new ++ defaultString;
			b.action = {arg field; func.value(true,field.value); l.close; };
		},prompt);
		{ b.focus }.defer;
	}
}
