TextView : QAbstractScroll {
	var <stringColor, <editable = true;

	*qtClass { ^'QcTextEdit' }

	enterInterpretsSelection { ^this.getProperty(\enterInterpretsSelection) }

	enterInterpretsSelection_ { |bool|
		this.setProperty(\enterInterpretsSelection, bool)
	}

	editable_ { |bool|
		editable = bool;
		this.setProperty(\readOnly, bool.not)
	}

	usesTabToFocusNextView_ { |bool|
		this.setProperty(\tabChangesFocus, bool)
	}

	open { |path|
		this.setProperty(\document, path)
	}

	string {
		^this.getProperty(\plainText)
	}

	string_ { |string|
		this.setProperty(\plainText, string)
	}

	font_ { |argFont|
		font = argFont;
		this.setProperty(\textFont, font)
	}

	stringColor_ { |color|
		stringColor = color;
		this.setProperty(\textColor, color)
	}

	background { ^this.palette.base }

	background_ { |color| this.palette = this.palette.base_(color) }

	currentLine {
		^this.getProperty(\currentLine)
	}

	selectedString {
		var string;
		string = this.getProperty(\selectedString);
		if(string.size == 0) { string = this.getProperty(\currentLine) };
		^string
	}

	selectedString_ { |string| this.setProperty(\selectedString, string) }

	selectionStart {
		^this.getProperty(\selectionStart)
	}

	selectionSize {
		^this.getProperty(\selectionSize)
	}

	select { |start, size|
		this.invokeMethod(\select, [start, size])
	}

	setStringColor { |color, start, size|
		this.setProperty(\rangeColor, [color, start, size])
	}

	setFont { |font, start, size|
		this.setProperty(\rangeFont, [font, start, size])
	}

	setString { |string, start, size|
		this.setProperty(\rangeText, [string, start, size])
	}

	tabWidth { ^this.getProperty(\tabStopWidth) }

	tabWidth_ { |pixels| this.setProperty(\tabStopWidth, pixels) }

	syntaxColorize {
		this.nonimpl("syntaxColorize")
	}

	defaultGetDrag {
		var text = this.selectedString;
		if(text.size < 1) { ^nil };
		if(text.size > 150) {
			this.dragLabel = text.copyRange(0, 149) ++ "..."
		} {
			this.dragLabel = text
		}
		^text
	}
}
