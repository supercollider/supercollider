Document {

	classvar <dir="", <allDocuments, thelistener;
	
	var <dataptr, path, title, visible, <background, <stringColor;
	var <>keyDownAction, <>onClose;
	var <isListener, <>toFrontAction, <>endFrontAction, <>mouseDownAction;
	
	*initClass {
		var num, listenernum, doc;
		num = this.numberOfOpen;
		num.do({arg i;
			doc = this.newFromIndex(i);
			doc.prisListener(false);
			allDocuments = allDocuments.add(doc);
		});
		listenernum = this.prGetIndexOfListener;	
		allDocuments.at(listenernum).prisListener(true);
		thelistener = allDocuments.at(listenernum);
	}
	
	*open { arg path, selectionStart=0, selectionLength=0;
		^super.new.initFromPath(path, selectionStart, selectionLength)
	}
	
	*new { arg str="", title="Untitled", makeListener=false;
		^super.new.initByString(str, title, makeListener);
	}
	
//class:
	*current {
		var idx;
		idx = this.prcurrentDocument;
		^allDocuments.at(idx);
	}

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
		var listenernum;
		if(thelistener.isNil,{
			listenernum = this.prGetIndexOfListener;
			thelistener = allDocuments.at(listenernum);
		});
		^thelistener
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
		line = line - 1;
		this.prselectLine(line);
	}
	
	selectRange {arg start=0, length=0;
		_TextWindow_SelectRange
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
	
	font_ {arg font, rangestart = -1, rangesize;
		this.setFont(font.name, font.size, rangestart, rangesize)
	}
		 
	selectedString_ {arg txt;
		this.prinsertText(txt)
	}
	
//actions:	
	didBecomeKey {
		toFrontAction.value(this);
	}
	
	didResignKey {
		endFrontAction.value(this);
	}
	
	mouseDown {
		mouseDownAction.value(this);
	}
	
	keyDown {arg character, modifiers, keycode;
		keyDownAction.value(character, modifiers, keycode);
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
	setFont {arg fontname="Monaco", size=9, rangeStart= -1, rangeSize=100;
		_TextWindow_SetFont
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
	}

	
	*prcurrentDocument {
		_TextWindow_Current
	}
	
	//check if there is already a document with the same pointer
	prAdd {
		
		if(allDocuments.isNil,{allDocuments = allDocuments.add(this);});
		allDocuments.do({arg doc;
			if(doc.dataptr == this.dataptr,{
				this = doc;
				^doc
			})
		});
		allDocuments = allDocuments.add(this);
		^this
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
		this.prinitByIndex(idx);
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
		isListener = false;
		^this.prAdd;
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
		^this.prAdd;
	}
	propen { arg path, selectionStart=0, selectionLength=0;
		_OpenTextFile
	}
	//private newTextWindow
	initByString{arg str, argTitle, makeListener;
		allDocuments = allDocuments.add(this);
		title = argTitle;
		if(makeListener, {
			allDocuments.do({arg doc; doc.prisListener(false)})
			});
		isListener = makeListener;
		thelistener = this;
		this.prinitByString(str, title, makeListener);
	
	}
	prinitByString { arg str, title, makeListener;
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
	
	prisListener { arg makeListener;
		isListener = makeListener;
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
	*new { arg envir, string="", title, pushNow=true;
		if(pushNow, { envir.push });
		title = envir.tryPerform(\name) ? "Untitled Environment";
		^super.new(string, "envir" + title.asString).envir_(envir)
				.background_(rgb(250, 240, 240));
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

