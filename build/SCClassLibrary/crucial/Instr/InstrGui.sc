
InstrGui : ObjectGui { 
	// this is a simple display of the instrument name and args
				
	guiBody { arg layout;
		var defs;
		
		// args
		CXLabel(layout,"ARGS:");
		model.argNames.do({ arg a,i;
			layout.startRow;		
			ArgNameLabel(  a ,layout,150);
			CXLabel(layout, " = " ++ model.defArgAt(i).asString,100);
			InspectorLink( model.specs.at(i) ,layout,300);
		});
		layout.startRow;
		CXLabel(layout,"outSpec");
		model.outSpec.gui(layout);
		// could show explicitly referenced classes in func def
		// offer to open the file
		ActionButton(layout,"edit File",{ model.path.openTextFile });
		ActionButton(layout,"make Patch",{ Patch(model.name).topGui });
	}

}
