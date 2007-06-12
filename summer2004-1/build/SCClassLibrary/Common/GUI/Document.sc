Document {

	classvar <dir="", <allDocuments, <>current;
	classvar <>globalKeyDownAction, <>initAction;

	//don't change the order of these vars:
	var <dataptr, <>keyDownAction, <>mouseDownAction, <>toFrontAction, <>endFrontAction;
	
	var path, title, visible, <background, <stringColor, <>onClose;
	var unused;
	var <editable;
	
	*initClass {
		var num, doc;
		num = this.numberOfOpen;
		num.do({arg i;
			doc = this.newFromIndex(i);
		});
	}
	
	*open { arg path, selectionStart=0, selectionLength=0;
		^super.new.initFromPath(path, selectionStart, selectionLength)
	}
	
	*new { arg title="Untitled", string="", makeListener=false;
		^super.new.initByString(title, string, makeListener);
	}
	
//class:

	*dir_ { arg path;  dir = path.standardizePath ++ "/"; }
	*standardizePath { arg p;
		var pathName;
		pathName = PathName.fromOS9(p.standardizePath);
		^if(pathName.isRelativePath,{
			dir ++ pathName.fullPath
		},{
			pathName.fullPath
		})
	}
	*abrevPath { arg path;
		if(path.size < dir.size,{ ^path });
		if(path.copyRange(0,dir.size - 1) == dir,{
			^path.copyRange(dir.size, path.size - 1)
		});
		^path
	}
	
	*openDocuments {
		^allDocuments
	}
	
	*hasEditedDocuments {
		allDocuments.do({arg doc, i;
			if(doc.isEdited, {
				^true;
			})
		})
		^false
	}
	
	*closeAll {arg leavePostWindowOpen = true;
		allDocuments.do({arg doc, i;
			if(leavePostWindowOpen.not, {
				doc.close;
			},{
				if(doc.isListener.not,{
					doc.close;
				})
			});
		})
	}
	
	*closeAllUnedited {arg leavePostWindowOpen = true;
		var listenerWindow;
		allDocuments.do({arg doc, i;
			if(doc.isListener,{ 
				listenerWindow = doc;
			},{
				if(doc.isEdited.not, {
					doc.close;
					});
			})
		});
		if(leavePostWindowOpen.not, {
			listenerWindow.close;
		})
	}
	
	*listener {
		^allDocuments[this.prGetIndexOfListener];
	}
	isListener {
		^allDocuments.indexOf(this) == this.prGetIndexOfListener
	}
	
//document setup	
	title {
		if(title.isNil,{title = this.prgetTitle})
		^title
	}
	
	title_ {arg argName;
		this.prSetName(argName);
	}
	
	background_ {arg color, rangestart= -1, rangesize = 0;
		background = color;
		this.setBackgroundColor(background, rangestart, rangesize);
	}
	
	stringColor_ {arg color, rangeStart = -1, rangeSize = 0;
		stringColor = color;
		this.setTextColor(color,rangeStart, rangeSize);
	}
	bounds {
		^this.prGetBounds(Rect.new);
	}
	bounds_ {arg argBounds;
		^this.prSetBounds(argBounds);
	}
//interaction:	

	close {
		this.prclose
	}
	
	front {
		_TextWindow_ToFront
	}
	
	unfocusedFront {
		_TextWindow_UnfocusedFront
	}
	
	syntaxColorize {
		_TextWindow_SyntaxColorize
	}
	
	selectLine {arg line;
		this.prselectLine(line);
	}
	
	selectRange {arg start=0, length=0;
		_TextWindow_SelectRange
	}
	editable_{arg abool=true;
		editable = abool;
		this.prisEditable_(abool);
	}
	prisEditable_{arg editable=true;
		_TextWindow_SetEditable
	}
	removeUndo{
		_TextWindow_RemoveUndo
	}
	
// state info
	isEdited {
		_TextWindow_IsEdited
	}
	isFront {
		^Document.current === this
	}
	
	selectionStart {
		^this.selectedRangeLocation
	}
	
	selectionSize {
		^this.selectedRangeSize
	}
	
	string {arg rangestart, rangesize = 1;
		if(rangestart.isNil,{
		^this.text;
		});
		^this.rangeText(rangestart, rangesize);
	}
	
	string_ {arg string, rangestart = -1, rangesize = 1;
		this.insertTextRange(string, rangestart, rangesize);
	}
	selectedString {
		^this.selectedText
	}
	
	
	font_ {arg font, rangestart = -1, rangesize=0;
		this.setFont(font, rangestart, rangesize)
	}
		 
	selectedString_ {arg txt;
		this.prinsertText(txt)
	}
	
	currentLine {
		var start, end, str, max;
		str = this.string;
		max = str.size;
		end = start = this.selectionStart;
		while { 
			str[start] !== Char.nl and: { start >= 0 }
		} { start = start - 1 };
		while { 
			str[end] !== Char.nl and: { end < max }
		} { end = end + 1 };
		^str.copyRange(start + 1, end);
	}
	
//actions:	
	didBecomeKey {
		this.class.current = this;
		toFrontAction.value(this);
	}
	
	didResignKey {
		endFrontAction.value(this);
	}
	
	mouseDown {
		mouseDownAction.value(this);
	}
	
	keyDown {arg character, modifiers, keycode;
		this.class.globalKeyDownAction.value(this,character, modifiers, keycode);
		keyDownAction.value(this,character, modifiers, keycode);
	}
	
//private-----------------------------------
	prSetName { arg argName;
		_TextWindow_SetName
		^this.primitiveFailed
	}
	prGetBounds { arg argBounds;
		_TextWindow_GetBounds
		^this.primitiveFailed
	}

	prSetBounds { arg argBounds;
		_TextWindow_SetBounds
		^this.primitiveFailed
	}

	//if range is -1 apply to whole doc
	setFont {arg font, rangeStart= -1, rangeSize=100;
		_TextWindow_SetFont
		^this.primitiveFailed

	}
	
	setTextColor { arg color,  rangeStart = -1, rangeSize = 0;
		_TextWindow_SetTextColor
	}
	
	text {
		_TextWindow_Text
	}
	selectedText {
		_TextWindow_SelectedText
	}
	
	rangeText { arg rangestart=0, rangesize=1; 
		_TextWindow_TextWithRange
	}
	
	prclose {
		_TextWindow_Close
	}

	closed {
		onClose.value(this); // call user function
		allDocuments.remove(this);
		dataptr = nil;
	}
	
	prinsertText { arg dataPtr, txt;
	 	_TextWindow_InsertText
	}
	insertTextRange { arg string, rangestart, rangesize;
		_TextWindow_InsertTextInRange
		^this.primitiveFailed
	}

	
	prAdd {
		allDocuments = allDocuments.add(this);
		this.editable = true;
		initAction.value(this);
	
	}
	
	//this is called after recompiling the lib
	*numberOfOpen {
		_NumberOfOpenTextWindows
	}
	
	*newFromIndex { arg idx;
		^super.new.initByIndex(idx)
	}
	initByIndex { arg idx;
		//allDocuments = allDocuments.add(this);
		var doc;
		doc = this.prinitByIndex(idx);
		if(doc.isNil,{this = nil; ^nil});
		this.prAdd;
	}
	prinitByIndex { arg idx;
		_TextWindow_GetByIndex
	}
	
	//this is called from the menu: open, new
	*prGetLast {
		^super.new.initLast
	}
	
	initLast {
		this.prGetLastIndex;
		if(dataptr.isNil,{ this = nil; ^nil});
		this.prAdd;
	}
	
	prGetLastIndex {
		_TextWindow_GetLastIndex
	}
	//private open
	initFromPath { arg apath, selectionStart, selectionLength;
		var stpath;
		path = apath;
		stpath = this.class.standardizePath(path);
		this.propen(stpath, selectionStart, selectionLength);
		if(dataptr.isNil,{ this = nil; ^nil});
		this.prAdd;
	}
	propen { arg path, selectionStart=0, selectionLength=0;
		_OpenTextFile
	}
	//private newTextWindow
	initByString{arg argTitle, str, makeListener;
	
		title = argTitle;
		this.prinitByString(title, str, makeListener);
		if(dataptr.isNil,{ this = nil; ^nil});
		this.prAdd;
	
	}
	prinitByString { arg title, str, makeListener;
		_NewTextWindow
	}
	//
	prgetTitle {
		_TextWindow_DisplayName
	}
	//other private
	//if -1 whole doc
	setBackgroundColor { arg color;
		_TextWindow_SetBackgroundColor
	}
	
	selectedRangeLocation {
		_TextWindow_GetSelectedRangeLocation
	}
	selectedRangeSize {
		_TextWindow_GetSelectedRangeLength
	}
	
	prselectLine { arg line;
		_TextWindow_SelectLine;
		^this.primitiveFailed
	}
	
	*prGetIndexOfListener {
		_TextWindow_GetIndexOfListener
	}
	//---not yet implemented
	// ~/Documents
	// /Volumes
	// Music/Patches
	
	//*reviewUnsavedDocumentsWithAlertTitle
	//*saveAllDocuments
	//*recentDocumentPaths
	//save
	//saveAs
	//print
	//
	//hasPath  was loaded
	
}


EnvirDocument : Document {
	var <>envir;
	*new { arg envir, title, string="", pushNow=true;
		if(pushNow) { envir.push };
		title = title ?? { "envir" + (envir.tryPerform(\name) ? "Untitled Environment") };
		^super.new(title, string).envir_(envir).background_(rgb(240, 240, 240));
	}
	
	didBecomeKey {
		envir.push;
		toFrontAction.value(this);
	}
	
	didResignKey {
		envir.pop;
		endFrontAction.value(this);
	}

}

