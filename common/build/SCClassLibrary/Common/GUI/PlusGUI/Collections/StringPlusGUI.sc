+ String {
	newTextWindow { arg title="Untitled", makeListener=false;
		Document.new(title, this, makeListener);
	}

	openTextFile{ arg selectionStart=0, selectionLength=0;
		var doc;
		doc = Document.open(PathName(this).asAbsolutePath , selectionStart, selectionLength);
	}
	openHTMLFile{ arg selectionStart=0, selectionLength=0;
		// On Linux this will be overridden to ensure it opens rendered HTML
		this.openTextFile(selectionStart, selectionLength)
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
	drawCenteredIn { arg rect, font, color;
		this.drawAtPoint(this.bounds( font ).centerIn(rect), font, color);
	}
	drawLeftJustIn { arg rect, font, color;
		var pos, bounds;
		bounds = this.bounds( font );
		pos = bounds.centerIn(rect);
		pos.x = rect.left + 2;
		this.drawAtPoint(pos, font, color);
	}
	drawRightJustIn { arg rect, font, color;
		var pos, bounds;
		bounds = this.bounds( font );
		pos = bounds.centerIn(rect);
		pos.x = rect.right - 2 - bounds.width;
		this.drawAtPoint(pos, font, color);
	}

	bounds { arg font;
		if(GUI.id === \swing,{
			// since Swing is not in svn and can't be easily updated
			// let's put this temporary hack/if-statement here
			// rather than pollute everybody else's code with hacks/if-statements
			^Rect(0, 0, this.size * font.size * 0.52146, font.size * 1.25)
			// width in Helvetica approx = string size * font size * 0.52146
			// 0.52146 is average of all 32-127 ascii characters widths
		},{
			^GUI.stringBounds(this, font)
		});
	}
	prBounds { arg rect, font;
		_String_GetBounds
		^this.primitiveFailed
	}


	findHelpFile {
		^if(thisProcess.platform.hasFeature(\findHelpFile)){
			// on osx, there's a primitve which does it nice and fast
			thisProcess.platform.findHelpFile(this)
		}{
			// this is very fast, but not on first run since it needs a tree to be scanned+built
			Help.findHelpFile(this)
		}

	}

	findHelpFileOrElse {
		// this is very fast, but not on first run since it needs a tree to be scanned+built
		^Help.findHelpFileOrElse(this)
	}

	openHelpFile {
		(this.findHelpFile ? "Help/Help.html".standardizePath).openHTMLFile
	}
}

