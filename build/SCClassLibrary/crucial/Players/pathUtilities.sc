
+ Object {
	loadPath {}
	enpath {}
	loadDocument {}
}

+ SequenceableCollection {
	loadPath {
		^this.collect({ arg obj;
			obj.loadPath
		})
	}
	enpath {
		^this.collect({ arg obj;
			obj.enpath
		})
	}
	loadDocument {
		^this.collect({ arg obj;
			obj.loadDocument
		})
	}
}

+ String {
	loadPath { arg warnIfNotFound=true;
		var obj,path;
		path = this.standardizePath;
		if(File.exists(path),{
			obj = thisProcess.interpreter.executeFile(path);
			obj.tryPerform(\path_,this);
		},{
			if(warnIfNotFound,{
				warn("String-loadPath file not found " + this + path);
			})
		});
		^obj
	}
	loadDocument { arg warnIfNotFound=true;
		var path;
		path = Document.standardizePath(this);
		^path.loadPath(warnIfNotFound)
			?? {ObjectNotFound.new(path)}
	}
	enpath {
		^Document.abrevPath(this)
	}
	openDocument {
		^Document(this).showWindow
	}
}

+ AbstractPlayer {
	
	enpath {
		//document enpath abrev it
		^if(path.notNil,{
			^Document.abrevPath(path)
		},{ 
			this
		})
	}
	loadDocument {
		^this
	}

// old style
	*load { arg obj, warnIfNotFound=true;
		^obj.loadDocument(warnIfNotFound);
	}
	*enpath { arg obj;
		^obj.enpath // document enpath
	}
	load { arg obj, warnIfNotFound=true;
		^this.class.load(obj,warnIfNotFound);
	}

}


