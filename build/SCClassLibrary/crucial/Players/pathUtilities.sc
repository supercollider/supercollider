
+ Object {
	loadPath {}
	enpath {}
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
}

+ AbstractPlayer {
	
	enpath {
		^path ? this
	}

}


