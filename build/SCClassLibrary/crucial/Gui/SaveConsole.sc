
SaveConsole : AbstractConsole {

	var <>object;
	var <path,<>defaultPath; // defaultPath may be a function
	var <>onSaveAs; // arg path
	
	*new { arg object, path,layout;
		^super.new.object_(object).path_(path)
			.layout_(layout.asPageLayout(object))
	}
	
	print {
		ActionButton(layout,"#",{ object.asCompileString.postln }); 
	}
	printPath {
		ActionButton(layout,"#path",{ path.value.asCompileString.postln })
	}
	//dirtyAwareSave
	save { arg title="save",maxx=100;
	 	/*
		ActionButton(layout,title,{
			if(path.value.isNil,{
		 		this.getPathThen(\doSaveAs);
		 	},{	
		 		this.doSave 
		 	})
	 	},maxx).backColor_(
	 		if(path.value.isNil,{ // virgin
	 			Color(202,255,161) 
	 		},{
	 			Color(255,242,89)
	 		})
	 	);
	 	*/
	}
	saveAs { arg onSaveF,default;
		/*
		defaultPath = default ? defaultPath;
		onSaveAs = onSaveF ? onSaveAs;
		ActionButton(layout,"save as",{
			this.getPathThen(\doSaveAs,default);
	 	});
	 	*/
	}
	load {
		//ActionButton(layout,"load",{ this.doLoad });
	}	
			
	getPathThen {  arg then ... args;
		/*
		var defPath;
		defPath=(defaultPath.value ? object).asString;
		defPath = defPath.copyRange(0,30); // mac beeps if path is too long
		File.saveDialog("Filename..." ++ then.asString, defPath, { arg  argpath;
			this.path = argpath;
			this.performList(then,[path] ++ args)
		});
		*/
	}
	
	doSave {
		var clobber,vpath;
		vpath = path.value;
		clobber=File.new(vpath,"r");
		if(clobber.length.notNil,{
			clobber.contents.write("~"++vpath);
		});
		clobber.close;	
		object.asCompileString.write(vpath);
	}
	doSaveAs {
		var clobber,vpath;
		vpath = path.value;
		clobber=File.new(vpath,"r");
		if(clobber.length.notNil,{
			clobber.contents.write("~"++vpath);
		});
		clobber.close;	
		object.asCompileString.write(vpath);
		onSaveAs.value(vpath);
	}

	doLoad { arg onLoad; // optional func
		/*
		File.openDialog("Load...",{ arg apath;
			this.path = apath;
			object=thisProcess.interpreter.executeFile(apath);
			onLoad.value(object,this)
		})
		*/
	}
	path_ { arg p; 
		if(p.notNil,{
			path = PathName(p).asRelativePath 
		})
	}
	
}

