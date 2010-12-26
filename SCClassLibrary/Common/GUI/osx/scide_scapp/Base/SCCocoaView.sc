SCTextView : SCView{
	var <stringColor, <font, <editable;
	var 	<autohidesScrollers, <hasHorizontalScroller, <hasVerticalScroller, <textBounds;
	var <usesTabToFocusNextView=true, <enterInterpretsSelection=true;
	var <>linkAction;

	mouseUp {arg x, y, modifiers, buttonNumber, clickCount, clickPos;
		mouseUpAction.value(this, x, y, modifiers, buttonNumber, clickCount, clickPos);	}

	doLinkAction { arg url, description;
		linkAction.value(this, url, description);
	}

	string{
		^this.getProperty(\string);
	}

	string_{|str|
		^this.setString(str, -1);
	}

	selectedString {
		^this.getProperty(\selectedString);
	}

	selectedString_{|str|
		this.setProperty(\selectedString, str);
	}

	selectionStart {
		^this.getProperty(\selectedRangeLocation);
	}

	selectionSize {
		^this.getProperty(\selectedRange);
	}

	path {
		^this.getProperty(\path);
	}

	stringColor_ {arg color;
		stringColor = color;
		this.setStringColor(color,-1, 0);
	}

	setStringColor{arg color, rangeStart = -1, rangeSize = 0;
		this.setProperty(\setTextColor, [color,rangeStart, rangeSize]);
	}
	
	syntaxColorize {
		this.setProperty(\setSyntaxColors);
	}
	
	font_ {arg afont;
		font = afont;
		this.setFont(font, -1, 0)
	}

	setFont{|	font, rangestart = -1, rangesize=0|
		this.setProperty(\setFont, [font, rangestart, rangesize]);
	}
	
	usesAutoInOutdent_{|bool|
		this.setProperty(\setUsesAutoInOutdent, bool);
	}

	setString{|string, rangestart=0, rangesize=0|
		this.setProperty(\insertStringInRange, [string, rangestart, rangesize]);
	}

	editable_{|bool|
		editable = bool;
		this.setProperty(\setEditable, bool);
	}

	enabled_{|bool|
		this.editable_(bool);
	}
	usesTabToFocusNextView_{|bool|
		usesTabToFocusNextView=bool;
		this.setProperty(\usesTabToFocusNextView, bool);
	}
	enterInterpretsSelection_{|bool|
		enterInterpretsSelection=bool;
		this.setProperty(\enterExecutesSelection, bool);
	}
	autohidesScrollers_{|bool|
		autohidesScrollers = bool;
		this.setProperty(\setAutohidesScrollers, bool);
	}
	hasHorizontalScroller_{|bool|
		hasHorizontalScroller = bool;
		this.setProperty(\setHasHorizontalScroller, bool);
	}
	hasVerticalScroller_{|bool|
		hasVerticalScroller = bool;
		this.setProperty(\setHasVerticalScroller, bool);
	}

	textBounds_{|rect|
		textBounds = rect;
		this.setProperty(\textBounds, rect);
	}

	open {|path|
		if ( path.contains( "SC://"), {
	        	path = Help.findHelpFile( path.asRelativePath( "SC:/") );
		});

		if ( path.contains( "://" ).not, {
	        if ( path.first.asString != "/" ) { path = String.scDir +/+ path; };
	        path = "file://"++path;
		});
		path = path.replace(" ", "%20");
		this.setProperty(\open, path);
	}

	*paletteExample{ arg parent, bounds;
		^this.new(parent, bounds).string_("The quick brown fox jumps over the lazy dog.");
	}
}

SCMovieView : SCView{
	var <rate, <loopMode, <muted, <path, <editable;

	/*
	loop modes
	0 Playback runs forward and backward between both endpoints.
	1 Restarts playback at beginning when end is reached.
	2 Playback stops when end is reached.
	*/

	start{
		this.setProperty(\start);
	}

	stop{
		this.setProperty(\stop);
	}

	path_{|moviePath|
		path = moviePath;
		this.setProperty(\setMovie, moviePath);
	}

	muted_{|bool|
		muted = bool;
		this.setProperty(\setMuted, bool);
	}

	playSelectionOnly_{|bool|
		this.setProperty(\setPlaysSelectionOnly, bool);
	}

	rate_{|ratein|
		rate = ratein;
		this.setProperty(\setRate, ratein);
	}

	loopMode_{|mode|
		loopMode = mode;
		this.setProperty(\setLoopMode, mode);
	}

	gotoEnd{
		this.setProperty(\gotoEnd);
	}
	stepForward{
		this.setProperty(\stepForward);
	}

	stepBack{
		this.setProperty(\stepBack);
	}

	gotoBeginning{
		this.setProperty(\gotoBeginning);
	}

	currentTime_{|time|
		this.setProperty(\setCurrentTime, time);
	}

	currentTime{
		//"SCMovieView-currentTime is not implemented".warn;
		^this.getProperty(\getCurrentTime);
	}

	editable_{|bool|
		editable = bool;
		this.setProperty(\setEditable, bool);
	}

	showControllerAndAdjustSize{|show, adjust|
		this.setProperty(\showControllerAndAdjustSize, [show, adjust]);
	}

	resizeWithMagnification{|size|
		this.setProperty(\resizeWithMagnification, size)
	}

	copy{
		this.setProperty(\copy);
	}
	clear{
		this.setProperty(\clear);
	}
	cut{
		this.setProperty(\cut);
	}
	paste{
		this.setProperty(\paste);
	}

	*paletteExample{ arg parent, bounds;
		^this.new(parent, bounds).path_("Help/GUI/Cocoa-GUI/SCImage/flowers2.jpg");
	}

}