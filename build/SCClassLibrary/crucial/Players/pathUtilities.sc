
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

