Document {

	classvar <dir="", <wikiDir="", <allDocuments, <>current;
	classvar <>globalKeyDownAction, <> globalKeyUpAction, <>initAction;
	
	classvar <>autoRun = true;
	classvar <>wikiBrowse = true;

	//don't change the order of these vars:
	var <dataptr, <>keyDownAction, <>keyUpAction, <>mouseDownAction, <>toFrontAction, <>endFrontAction, <>onClose;
	
	var <stringColor;
// 	var >path, visible; 
// replaced by primitves: title, background
	var <editable;
	
	*startup {
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
		^super.new.initByString(title, string.asString, makeListener);
	}
	
//class:

	*dir_ { arg path; path = path.standardizePath;
		if(path == "") {dir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				dir = path ++ "/"
			}
		}
	}
	
	*wikiDir_ { arg path; path = path.standardizePath;
		if(path == "") {wikiDir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				wikiDir = path ++ "/"
			}
		}
	}
	
	*standardizePath { arg p;
		var pathName;
		pathName = PathName.fromOS9(p.standardizePath);
		^if(pathName.isRelativePath,{
			dir  ++ pathName.fullPath
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
		^allDocuments.indexOf(this) == this.class.prGetIndexOfListener
	}
	
//document setup	
	path{
		^this.prGetFileName
	}
	path_{|apath|
		this.prSetFileName(apath);
	}
	dir { var path = this.path; ^path !? { path.dirname } }	
	title {
		^this.prGetTitle
	}
	
	title_ {arg argName;
		this.prSetTitle(argName);
	}
	
	background_ {arg color;
		this.prSetBackgroundColor(color);
	}
	background{
		var color;
		color = Color.new;
		this.prGetBackgroundColor(color);
		^color;
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
	
	alwaysOnTop_{|boolean=true|
		_TextWindow_AlwaysOnTop
	}
	
	alwaysOnTop{
		_TextWindow_IsAlwaysOnTop
	}
		
	syntaxColorize {
		_TextWindow_SyntaxColorize
	}
	
	selectLine {arg line;
		this.prSelectLine(line);
	}
	
	selectRange {arg start=0, length=0;
		_TextWindow_SelectRange
	}
	
	editable_{arg abool=true;
		editable = abool;
		this.prIsEditable_(abool);
	}
	removeUndo{
		_TextWindow_RemoveUndo
	}
	
	promptToSave_{|bool|
		_TextWindow_SetPromptToSave
	}
	
	promptToSave{
		_TextWindow_PromptToSave
	}	
	
	underlineSelection{
		_TextWindow_UnderlineSelection
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
	
	makeWikiPage { arg wikiWord, extension=".rtf";
		var filename, file, doc, string, dir;
		
		filename = wikiDir ++ wikiWord ++ extension;
		file = File(filename, "w");
		if (file.isOpen) {
			string = "{\\rtf1\\mac\\ansicpg10000\\cocoartf102\\n{\\fonttbl}\n"
				"{\\colortbl;\\red255\\green255\\blue255;}\n"
				"Write about " ++ wikiWord ++ " here.\n}";
			file.write(string);
			file.close;
			
			doc = this.class.open(filename);
			doc.path = filename;
			doc.selectRange(0,0x7FFFFFFF);
			doc.onClose = {
				if(doc.string == ("Write about " ++ wikiWord ++ " here.")) {  
					unixCmd("rm" + filename) 
				};
			};
		} {
			// in a second try, check if a path must be created.
			// user makes double click on string.
			dir = wikiWord.dirname;
			if(dir != ".") {
				dir = wikiDir ++ dir;
				"created directory: % \n".postf(dir); 
				unixCmd("mkdir -p" + dir); 
			};
		}
	}
	openWikiPage {
		var selectedText, filename, index;
		var extensions = #[".rtf", ".sc", ".txt", ""];
		selectedText = this.selectedText;
		index = this.selectionStart;
		this.selectRange(index, 0);
		case { selectedText[0] == $* }
		{ 
			// execute file
			selectedText = selectedText.drop(1);
			extensions.do {|ext|
				filename = wikiDir ++ selectedText ++ ext;
				if (File.exists(filename)) {
					// open existing wiki page
					filename.load;
					^this
				}
				{
				filename = "Help/help-scripts/" ++ selectedText ++ ext;
				if (File.exists(filename)) {
					// open help-script document
					filename.load;
					^this
				}
				}
			};
		}
		{ selectedText.first == $[ and: { selectedText.last == $] }}
		{
			// open help file
			selectedText[1 .. selectedText.size-2].openHelpFile
		}
		{ selectedText.containsStringAt(0, "http://") 
			or: { selectedText.containsStringAt(0, "file://") } }
		{
			// open URL
			("open " ++ selectedText).unixCmd;
		}
		{ selectedText.containsStringAt(selectedText.size-1, "/") }
		{
			Document(selectedText, 
				pathMatch(wikiDir ++ selectedText).collect({|it|it.basename ++ "\n"}).join
			)
		}
		{
			if(index + selectedText.size > this.text.size) { ^this };
			extensions.do {|ext|
				filename = wikiDir ++ selectedText ++ ext;
				if (File.exists(filename)) {
					// open existing wiki page
					this.class.open(filename);
					^this
				}
			};
			// make a new wiki page
			this.makeWikiPage(selectedText);
		};
	}
		
	mouseDown { arg clickPos;	
		mouseDownAction.value(this);
		if (wikiBrowse and: { this.selectUnderlinedText(clickPos) }) {
			^this.openWikiPage
		};		
	}
	
	keyDown {arg character, modifiers, keycode;
		this.class.globalKeyDownAction.value(this,character, modifiers, keycode);
		keyDownAction.value(this,character, modifiers, keycode);
	}

	keyUp {arg character, modifiers, keycode;
		this.class.globalKeyUpAction.value(this,character, modifiers, keycode);
		keyUpAction.value(this,character, modifiers, keycode);
	}
		
	== { arg doc;
		^if(this.path.isNil or: { doc.path.isNil }) { doc === this } {
			this.path == doc.path
		}
	}
	
//private-----------------------------------
	prIsEditable_{arg editable=true;
		_TextWindow_SetEditable
	}
	prSetTitle { arg argName;
		_TextWindow_SetName
		^this.primitiveFailed
	}
	prGetTitle {
		_TextWindow_GetName
	}	
	prGetFileName {
		_TextWindow_GetFileName
		^this.primitiveFailed
	}
	prSetFileName {|apath|
		_TextWindow_SetFileName
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
	selectUnderlinedText { arg clickPos;
		_TextWindow_SelectUnderlinedText
		^false
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
		if (autoRun) {
			if (this.rangeText(0,7) == "/*RUN*/")
			{
				this.text.interpret;
			}
		};
		current = this;
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
		if(doc.isNil,{^nil});
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
		if(dataptr.isNil,{^nil});
		this.background_(Color.white);
		this.prAdd;
	}
	
	prGetLastIndex {
		_TextWindow_GetLastIndex
	}
	//private open
	initFromPath { arg path, selectionStart, selectionLength;
		var stpath;
//		path = apath;
		stpath = this.class.standardizePath(path);
		this.propen(stpath, selectionStart, selectionLength);
		if(dataptr.isNil,{ 
			^this.class.allDocuments.do{|d| 
					if(d.path == stpath.absolutePath){
						^d
					}
				}
		});
		this.background_(Color.white);
		^this.prAdd;
	}
	propen { arg path, selectionStart=0, selectionLength=0;
		_OpenTextFile
	}
	//private newTextWindow
	initByString{arg argTitle, str, makeListener;
	
		this.prinitByString(argTitle, str, makeListener);
		this.background_(Color.white);		
		if(dataptr.isNil,{^nil});
		this.prAdd;
		this.title = argTitle;
	
	}
	prinitByString { arg title, str, makeListener;
		_NewTextWindow
	}

	//other private
	//if -1 whole doc
	prSetBackgroundColor { arg color;
		_TextWindow_SetBackgroundColor
	}
	prGetBackgroundColor { arg color;
		_TextWindow_GetBackgroundColor
	}	
	selectedRangeLocation {
		_TextWindow_GetSelectedRangeLocation
	}
	selectedRangeSize {
		_TextWindow_GetSelectedRangeLength
	}
	
	prSelectLine { arg line;
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
	
	*new { arg envir, title="- Untitled Environment", string="";
		^super.new(title, string).envir_(envir)
	}
	
	*open { arg envir, path, selectionStart=0, selectionLength=0;
		var doc = super.open(path, selectionStart, selectionLength);
		^doc !? { doc.envir_(envir) }	}
	
	didBecomeKey {
		envir !? { envir.push };
		this.class.current = this;
		toFrontAction.value(this);
	}
	
	didResignKey {
		envir !? { envir.pop };
		endFrontAction.value(this);
	}

}

