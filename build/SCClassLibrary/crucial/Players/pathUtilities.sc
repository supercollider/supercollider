
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
		^Document.standardizePath(this).loadPath(warnIfNotFound)
	}
}

+ AbstractPlayer {
	
	enpath {
		//abrev it
		^path ? this
	}
	loadDocument {
		^this
	}

// old style
	*load { arg obj, warnIfNotFound=true;
		^obj.loadDocument;
	}
	*enpath { arg obj;
		^obj.enpath // document enpath
	}
	load { arg obj, warnIfNotFound=true;
		^this.class.load(obj,warnIfNotFound);
	}
	enpath { arg obj;
		^this.class.enpath(obj) // document enpath
	}

}


