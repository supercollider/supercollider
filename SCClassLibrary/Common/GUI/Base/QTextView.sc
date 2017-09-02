TextView : QAbstractScroll {
	var <stringColor, <editable=true;

	*qtClass { ^'QcTextEdit' }

	enterInterpretsSelection { ^this.getProperty( \enterInterpretsSelection ); }

	enterInterpretsSelection_ { arg bool;
		this.setProperty( \enterInterpretsSelection, bool );
	}

	editable_ { arg bool;
		editable = bool;
		this.setProperty( \readOnly, bool.not );
	}

	usesTabToFocusNextView_ { arg bool;
		this.setProperty( \tabChangesFocus, bool );
	}

	open { arg path;
		this.setProperty( \document, path );
	}

	string {
		^this.getProperty( \plainText );
	}

	string_ { arg string;
		this.setProperty( \plainText, string );
	}

	font_ { arg argFont;
		font = argFont;
		this.setProperty( \textFont, font );
	}

	stringColor_ { arg color;
		stringColor = color;
		this.setProperty( \textColor, color );
	}

	background { ^this.palette.base }

	background_ { arg color; this.palette = this.palette.base_(color); }

	currentLine {
		^this.getProperty( \currentLine );
	}

	selectedString {
		var string;
		string = this.getProperty( \selectedString );
		if (string.size == 0) { string = this.getProperty( \currentLine ) };
		^string;
	}

	selectedString_ { arg string; this.setProperty( \selectedString, string ); }

	selectionStart {
		^this.getProperty( \selectionStart );
	}

	selectionSize {
		^this.getProperty( \selectionSize );
	}

	select { arg start, size;
		this.invokeMethod( \select, [start, size] );
	}

	setStringColor { arg color, start, size;
		this.setProperty( \rangeColor, [color, start, size] );
	}

	setFont { arg font, start, size;
		this.setProperty( \rangeFont, [font, start, size] );
	}

	setString { arg string, start, size;
		this.setProperty( \rangeText, [string, start, size] );
	}

	tabWidth { ^this.getProperty( \tabStopWidth ); }

	tabWidth_ { arg pixels; this.setProperty( \tabStopWidth, pixels ); }

	syntaxColorize {
		this.nonimpl( "syntaxColorize" );
	}

	defaultGetDrag {
		var text = this.selectedString;
		if( text.size < 1 ) { ^nil };
		if( text.size > 150 ) {
			this.dragLabel = text.copyRange(0,149) ++ "...";
		}{
			this.dragLabel = text;
		}
		^text;
	}
}
