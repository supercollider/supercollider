+ String {
	newTextWindow { arg title="Untitled", makeListener=false;
		Document.new(title, this, makeListener);
	}

	openHTMLFile{ arg selectionStart=0, selectionLength=0;
		var webView;

		if (Platform.openHTMLFileAction.notNil) {
			Platform.openHTMLFileAction.value(this, selectionStart, selectionLength);
			^this
		};

		webView = \QWebView.asClass;
		if (webView.notNil) {
			webView = webView.new;
			webView.url_(this);
			webView.front;
			^this;
		};

		this.openDocument(selectionStart, selectionLength)
	}

	openDocument { arg selectionStart=0, selectionLength=0;
		var ideClass;
		if(Document.implementationClass.notNil) {
			Document.open(this, selectionStart, selectionLength);
			^this
		};

		ideClass = \ScIDE.asClass;
		if ( ideClass.notNil ) {
			if ( this.endsWith(".sc") || this.endsWith(".scd") ) {
				ideClass.open(this, selectionStart, selectionLength);
				^this
			}
		};

		this.openOS
	}

	draw {
		this.drawAtPoint(Point(0,0), Font.default, Color.black);
	}
	drawAtPoint { arg point, font, color;
		if(GUI.id === \cocoa)
			{ this.prDrawAtPoint( point, font, color ) }
			{ Pen.stringAtPoint( this, point, font, color ) }
	}
	drawInRect { arg rect, font, color;
		if(GUI.id === \cocoa)
			{ this.prDrawInRect( rect, font, color ) }
			{ Pen.stringInRect( this, rect, font, color ) }
	}
	prDrawAtPoint { arg point, font, color;
		_String_DrawAtPoint
		^this.primitiveFailed
	}
	prDrawInRect { arg rect, font, color;
		_String_DrawInRect
		^this.primitiveFailed
	}
	drawCenteredIn { arg rect, font, color;
		if(GUI.id === \cocoa)
			{ this.drawAtPoint(this.bounds( font ).centerIn(rect), font, color) }
			{ Pen.stringCenteredIn( this, rect, font, color ) }
	}
	drawLeftJustIn { arg rect, font, color;
		var pos, bounds;
		if(GUI.id === \cocoa)
			{
				bounds = this.bounds( font );
				pos = bounds.centerIn(rect);
				pos.x = rect.left + 2;
				this.drawAtPoint(pos, font, color);
			}
			{ Pen.stringLeftJustIn( this, rect, font, color ) }
	}
	drawRightJustIn { arg rect, font, color;
		var pos, bounds;
		if(GUI.id === \cocoa)
			{
				bounds = this.bounds( font );
				pos = bounds.centerIn(rect);
				pos.x = rect.right - 2 - bounds.width;
				this.drawAtPoint(pos, font, color);
			}
			{ Pen.stringRightJustIn( this, rect, font, color ) }
	}

	bounds { arg font;
		if(GUI.id === \swing,{
			// since Swing is not in svn and can't be easily updated
			// let's put this temporary hack/if-statement here
			// rather than pollute everybody else's code with hacks/if-statements
			font = font ?? { Font.default };
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
		^SCDoc.findHelpFile(this);
	}

	findHelpFileOrElse {
		this.findHelpFile;
	}

	help {
		if (Platform.openHelpFileAction.notNil) {
			Platform.openHelpFileAction.value(this)
		} {
			HelpBrowser.openHelpFor(this);
		}
	}
}

+ Symbol {
	openDocument { arg selectionStart=0, selectionLength=0;
		^this.asString.openDocument(selectionStart, selectionLength)
	}
}
