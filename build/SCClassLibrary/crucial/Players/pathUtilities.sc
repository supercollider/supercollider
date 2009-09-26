
+ Object {
	loadPath {}
	enpath {}
	loadDocument {}
	didLoadFromPath {}
}

+ SequenceableCollection {
	loadPath { arg warnIfNotFound=true;		^this.collect { arg obj;			obj.loadPath(warnIfNotFound)		}	}
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
			{
				obj = thisProcess.interpreter.executeFile(path);
				obj.didLoadFromPath(this);
			}.try({ arg err;
				("In file: " + this).postln;
				err.throw;
			});
		},{
			if(warnIfNotFound,{
				warn("String:loadPath file not found " + this + path);
				^nil
			});
		});
		if(obj.isNil and: warnIfNotFound, {
			warn("String:loadPath found nil, empty contents or parse error in " + path);

			//^ObjectNotFound.new(path)
		});
		^obj
	}
	loadDocument { arg warnIfNotFound=true;
		var path,obj;
		path = Document.standardizePath(this);
		if(File.exists(path),{
			obj = thisProcess.interpreter.executeFile(path);
			obj.didLoadFromPath(path);
			^obj
		},{
			if(warnIfNotFound,{
				warn("String:loadDocument file not found " + this + path);
			});
			^ObjectNotFound.new(path)
		});
	}
	enpath {
		^Document.abrevPath(this)
	}

	guiDocument {
		var doc;
		doc = this.loadDocument;
		doc.gui;
		^doc
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
	didLoadFromPath { arg argpath;
		path = argpath;
		//dirty = false;
	}
// old style
	*load { arg obj, warnIfNotFound=true;
		^obj.loadDocument(warnIfNotFound);
	}
	*enpath { arg obj;
		^obj.enpath // document enpath
	}
	load { arg obj, warnIfNotFound=true;
		^obj.loadDocument(warnIfNotFound);
	}

}





/*

	converts any signed integer (such as an identityHash) into a base 62
	string for unique file name usage.
*/

+ Integer {

	asFileSafeChar {
		^(#[ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z","_","*","#","-","+","~","$" ]).at(this)
	}
	/*
		extension methods crash if called recursively.
		james is working on a fix.

	asFileSafeString {
		var output,msd,lsd;
		if(this < 62,{
			if(this.isNegative,{
				^("-" ++ this.neg.asFileSafeString)
			},{
				^this.asFileSafeChar
			})
		},{
			msd = floor(this / 62.0);
			lsd = this - (msd * 62.0);
			if(msd >= 62,{
				^msd.asFileSafeString ++ lsd.asInteger.asFileSafeChar
			},{
				^msd.asInteger.asFileSafeChar ++ lsd.asInteger.asFileSafeChar
			})
		})
	}
	*/
	asFileSafeString {
		var output = "", val, msd, lsd;
		val = this.abs;
		while ({ val != 0 },{
			msd = floor(val / 69).asInteger;
			lsd = val - (msd * 69);
			output = lsd.asFileSafeChar ++ output;
			val = msd;
		});
		if (this < 0, { ^"-" ++ output });
		^output
	}

}

+ Object {

	asFileSafeString {
		^this.asCompileString.hash.asFileSafeString
	}

}

+ String {
	asFileSafeString { // this is more for protecting against / etc.
		^this.collect({ arg c;
			if(c.isFileSafe,{ c },{ $# })
		})
	}
}

