
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
	loadPath {
		var obj;
		obj = thisProcess.interpreter.executeFile(this.standardizePath);
		if(obj.isNil,{
			die("String-load failed loading string assumed to be path " + this);
		},{
			obj.tryPerform(\path_,this);
		});
		^obj
	}
}

+ AbstractPlayer {
	
	enpath {
		^path ? this
	}

}


