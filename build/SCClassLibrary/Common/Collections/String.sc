
String[char] : RawArray {
	
	asSymbol { 
		_StringAsSymbol 
		^this.primitiveFailed
	}
	asInteger { 
		_String_AsInteger
		^this.primitiveFailed
	}
	asFloat { 
		_String_AsFloat
		^this.primitiveFailed
	}
	
	compare { arg aString; _StringCompare }
	< { arg aString; ^this.compare(aString) < 0 }
	== { arg aString; ^this.compare(aString) == 0 }
	!= { arg aString; ^this.compare(aString) != 0 }
	hash { _StringHash }
	
	// no sense doing collect as per superclass collection
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; 
		^this.perform(aSelector, aNumber)
	}
	performBinaryOpOnComplex { arg aSelector, aComplex; 
		^this.perform(aSelector, aComplex)
	}
	performBinaryOpOnInfinitum { arg aSelector, aNumber; 
		^this.perform(aSelector, aNumber)
	}

	
	isString { ^true }
	asString { ^this }
	asCompileString { ^"\"" ++ this ++ "\"" }
	species { ^String }
	
	postln { _PostLine }
	post { _PostString }
	postcln { "// ".post; this.postln; }
	postc { "// ".post; this.post; }

	newTextWindow { arg title="Untitled", makeListener=false;
		_NewTextWindow 
		^this.primitiveFailed
	}
	openTextFile { arg selectionStart=0, selectionLength=0;
		_OpenTextFile 
		^this.primitiveFailed
	}
	
//	*fromUser { arg prompt="Enter string :", default="";
//		_GetStringFromUser
//		^this.primitiveFailed
//	}
	
	die { "FATAL ERROR:\n".post;  this.postln;  this.halt; }
	error { "ERROR:\n".post; this.postln; }
	warn { "WARNING:\n".post; this.postln }
	inform { ^this.postln }
	++ { arg anObject; ^this prCat: anObject.asString; }
	+ { arg anObject; ^this prCat: " " prCat: anObject.asString; }
	catArgs { arg ... items; ^this.catList(items) }
	scatArgs { arg ... items; ^this.scatList(items) }
	ccatArgs { arg ... items; ^this.scatList(items) }
	catList { arg list; 
		// concatenate this with a list as a string
		var string;
		string = this.copy;
		list.do({ arg item, i;
			string = string ++ item;
		});
		^string
	}
	scatList { arg list; 
		var string;
		string = this.copy;
		list.do({ arg item, i;
			string = string primCat: " " ++ item;
		});
		^string
	}
	ccatList { arg list; 
		var string;
		string = this.copy;
		list.do({ arg item, i;
			string = string primCat: ", " ++ item;
		});
		^string
	}

	compile { ^thisProcess.interpreter.compile(this); }
	interpret { ^thisProcess.interpreter.interpret(this); } 
	interpretPrint { ^thisProcess.interpreter.interpretPrint(this); }
	
	draw {
		this.drawAtPoint(Point(0,0), Font.default, Color.black);
	}
	drawAtPoint { arg point, font, color;
		_String_DrawAtPoint
		^this.primitiveFailed
	}
	drawInRect { arg rect, font, color;
		_String_DrawInRect
		^this.primitiveFailed
	}
	drawCenteredIn { arg inRect;
		this.drawAtPoint(this.bounds.centerIn(inRect).origin);
	}
	drawLeftJustIn { arg inRect;
		var pos, bounds;
		bounds = this.bounds;
		pos = bounds.centerIn(inRect);
		pos.x = inRect.left + 2;
		this.drawAtPoint(pos);
	}
	drawRightJustIn { arg inRect;
		var pos, bounds;
		bounds = this.bounds;
		pos = bounds.centerIn(inRect);
		pos.x = inRect.right - 2 - bounds.width;
		this.drawAtPoint(pos);
	}
	
	*readNew { arg file;
		^file.readAllString;
	}
	prCat { arg aString; _ArrayCat }
	
	printOn { arg stream;
		stream.putAll(this);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}
	
//	bounds { ^this.prBounds(Rect.new) }
//	getLayoutSize {
//		arg extent;
//		extent = this.bounds.extent;
//		^LayoutSize(extent.x+1, extent.x+1, extent.y+1, extent.y+1, 0);
//	}
//	prBounds { arg rect;
//		_String_GetBounds
//		^this.primitiveFailed
//	}
	
	inspectorClass { ^StringInspector }
	
	/// unix
	
	// runs a unix command and returns the result code.
	//unixCmd { _String_System ^this.primitiveFailed }
	
	// runs a unix command and sends stdout to the post window
	unixCmd { _String_POpen ^this.primitiveFailed }

	gethostbyname { _GetHostByName ^this.primitiveFailed }
	
	/// code gen
	codegen_UGenCtorArg { arg stream; 
		stream << this.asCompileString; 
	}
	ugenCodeString { arg ugenIndex, isDecl, inputNames=#[], inputStrings=#[];
		_UGenCodeString
		^this.primitiveFailed
	}
	
	/// os x
	standardizePath { _Cocoa_StandardizePath ^this.primitiveFailed }
	
}

