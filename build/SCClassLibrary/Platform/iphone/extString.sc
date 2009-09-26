+ String
{
	loadPath { arg warnIfNotFound=true;
		var obj,path;
		path = this.standardizePath;
		if(File.exists(path),{
			{
				obj = thisProcess.interpreter.executeFile(path);
				//obj.didLoadFromPath(this);
			}.try({ arg err;
				("In file: " + this).postln;
				err.throw;
			});
		},{
			if(warnIfNotFound,{
				warn("String:loadPath file not found " + this + path);
			});
		});
		if(obj.isNil and: warnIfNotFound, {
			warn("String:loadPath found nil, empty contents or parse error in " + path);

			//^ObjectNotFound.new(path)
		});
		^obj
	}
}