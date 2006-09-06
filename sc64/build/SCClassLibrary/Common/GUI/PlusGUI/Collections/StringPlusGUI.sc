+ String {
	newTextWindow { arg title="Untitled", makeListener=false;
		Document.new(title, this, makeListener);
	}
	
	openTextFile{ arg selectionStart=0, selectionLength=0;
		var doc;
		doc = Document.open(PathName(this).asAbsolutePath , selectionStart, selectionLength);
	}
	
	hackOpenWinTextFile { arg path, rangeStart, rangeSize; 
		_OpenWinTextFile ;//doc = Document.open(PathName(this).asAbsolutePath , selectionStart, selectionLength);
	}
	   
	openWinTextFile{ arg selectionStart=0, selectionLength=0;
		//hackOpenWinTextFile(PathName(this).asAbsolutePath , selectionStart, selectionLength); // standardizePath on win32 yet
		hackOpenWinTextFile(this, selectionStart, selectionLength);
	}

	openDocument {
		^Document.open(this)
	}	
//	*fromUser { arg prompt="Enter string :", default="";
//		_GetStringFromUser
//		^this.primitiveFailed
//	}
	
	
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
		
	bounds { ^this.prBounds(Rect.new) }
	prBounds { arg rect;
		_String_GetBounds
		^this.primitiveFailed
	}
//	getLayoutSize {
//		arg extent;
//		extent = this.bounds.extent;
//		^LayoutSize(extent.x+1, extent.x+1, extent.y+1, extent.y+1, 0);
//	}

	/// cocoa or at least foundation dependant
	standardizePath { _Cocoa_StandardizePath ^this.primitiveFailed }
	absolutePath{
		var first;
		first = this[0];
		if(first == $/){^this};
		if(first == $~){^this.standardizePath};
		^File.getcwd ++ "/" ++ this;
	}
	findHelpFile {
		_Cocoa_HelpFileForString_
		^this.primitiveFailed
	}
	openHelpFile {
		(this.findHelpFile ? "Help/Help.help.rtf".standardizePath).openTextFile
	}
}
