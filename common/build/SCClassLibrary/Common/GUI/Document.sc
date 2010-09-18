// Since SC v3.2 dev, Document is an ABSTRACT class. Can't be instantiated directly.
// Subclasses provide the editor-specific implementation, e.g. CocoaDocument for the standard Mac interface.
// Subclasses also (in their SC code files) add a "implementationClass" method to Document to tell it to use them.

Document {

	classvar <dir="", <wikiDir="", <allDocuments, >current;
	classvar <>globalKeyDownAction, <> globalKeyUpAction, <>initAction;

	classvar <>autoRun = true;
	classvar <>wikiBrowse = true;

	classvar <>implementationClass;
	classvar <postColor;
	classvar <theme, <themes;

	//don't change the order of these vars:
	var <dataptr, <>keyDownAction, <>keyUpAction, <>mouseUpAction;
	var <>toFrontAction, <>endFrontAction, <>onClose, <>mouseDownAction;

	var <stringColor;

	var <envir, savedEnvir;
	var <editable;

	  *initClass{
		allDocuments = [];
	  }

	*startup {
		var num, doc;
		num = this.numberOfOpen;
		num.do { | i |
			doc = this.newFromIndex(i);
		};
		postColor = Color.black;
		themes = (
			default: (
				classColor: Color(0, 0, 0.75, 1),
				textColor: Color(0, 0, 0, 1),
				stringColor: Color(0.375, 0.375, 0.375, 1),
				commentColor: Color(0.75, 0, 0, 1),
				symbolColor: Color(0, 0.45, 0, 1),
				numberColor: Color(0, 0, 0, 1)
				)
			);
		theme = themes.default;
	}

	*open { | path, selectionStart=0, selectionLength=0, envir |
		var doc, env;
		env = currentEnvironment;
		if(this.current.notNil) { this.current.restoreCurrentEnvironment };
		doc = Document.implementationClass.prBasicNew.initFromPath(path, selectionStart, selectionLength);
		if (doc.notNil) {
			doc.envir_(envir)
		} {
			currentEnvironment = env
		};
		^doc
	}

	*new { | title="Untitled", string="", makeListener=false, envir |
		var doc, env;
		env = currentEnvironment;
		if(this.current.notNil) { this.current.restoreCurrentEnvironment };
		doc = Document.implementationClass.new(title, string, makeListener);
		if (doc.notNil) {
			doc.envir_(envir)
		} {
			currentEnvironment = env
		};
		^doc
	}

	*dir_ { | path |
		path = path.standardizePath;
		if(path == "") { dir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				dir = path ++ "/"
			}
		}
	}

	*wikiDir_ { | path |
		path = path.standardizePath;
		if(path == "") {wikiDir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				wikiDir = path ++ "/"
			}
		}
	}

	*standardizePath { | p |
		var pathName;
		pathName = PathName(p.standardizePath);
		^if(pathName.isRelativePath) {
			dir  ++ pathName.fullPath
		} {
			pathName.fullPath
		}
	}
	*abrevPath { | path |
		if(path.size < dir.size) { ^path };
		if(path.copyRange(0,dir.size - 1) == dir) {
			^path.copyRange(dir.size, path.size - 1)
		};
		^path
	}

	*openDocuments {
		^allDocuments
	}

	*hasEditedDocuments {
		allDocuments.do { | doc, i |
			if(doc.isEdited) {
				^true;
			}
		}
		^false
	}

	*closeAll { | leavePostWindowOpen = true |
		allDocuments.do { | doc, i |
			if(leavePostWindowOpen.not) {
				doc.close;
			} {
				if(doc.isListener.not) {
					doc.close;
				}
			}
		}
	}

	*closeAllUnedited { | leavePostWindowOpen = true |
		var listenerWindow;
		allDocuments.do({ | doc, i |
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

	*current {
		if ( thisProcess.platform.hasFeature( \emacs ), {
			^this.implementationClass.current;
		});
		^current;
	}

	*listener {
		^allDocuments[this.implementationClass.prGetIndexOfListener];
	}
	isListener {
		^allDocuments.indexOf(this) == this.class.prGetIndexOfListener
	}

// document setup

	path {
		^this.prGetFileName
	}
	path_ { |apath|
		this.prSetFileName(apath);
	}
	dir { var path = this.path; ^path !? { path.dirname } }
	name {
		^this.title
	}

	name_ { |aname|
		this.title_(aname)
	}

	title {
		^this.prGetTitle
	}

	title_ { | argName |
		this.prSetTitle(argName);
	}

	background_ { | color |
		this.prSetBackgroundColor(color);
	}
	background {
		var color;
		color = Color.new;
		this.prGetBackgroundColor(color);
		^color;
	}

	selectedBackground_ { | color |
		this.prSetSelectedBackgroundColor(color);
	}

	selectedBackground {
		var color;
		color = Color.new;
		this.prGetSelectedBackgroundColor(color);
		^color;
	}

	*postColor_ { | col |
		postColor = col;
		^Document.implementationClass.postColor_(col);
	}

	stringColor_ { | color, rangeStart = -1, rangeSize = 0 |
		stringColor = color;
		this.setTextColor(color,rangeStart, rangeSize);
	}
	bounds {
		^this.prGetBounds(Rect.new);
	}
	bounds_ { | argBounds |
		^this.prSetBounds(argBounds);
	}

// interaction:

	close {
		this.prclose
	}

	front {
		^this.subclassResponsibility(thisMethod)
	}

	unfocusedFront {
		^this.subclassResponsibility(thisMethod)
	}

	alwaysOnTop_ { |boolean=true|
		^this.subclassResponsibility(thisMethod)
	}

	alwaysOnTop {
		^this.subclassResponsibility(thisMethod)
	}

	syntaxColorize {
		^this.subclassResponsibility(thisMethod)
	}

	selectLine { | line |
		this.prSelectLine(line);
	}

	selectRange { | start=0, length=0 |
		^this.subclassResponsibility(thisMethod)
	}

	editable_ { | abool=true |
		editable = abool;
		this.prIsEditable_(abool);
	}
	removeUndo {
		^this.subclassResponsibility(thisMethod)
	}

	promptToSave_ { | bool |
		^this.subclassResponsibility(thisMethod)
	}

	promptToSave {
		^this.subclassResponsibility(thisMethod)
	}

	underlineSelection {
		^this.subclassResponsibility(thisMethod)
	}

	*setTheme { | themeName |
		theme = themes[themeName];
		Document.implementationClass.prSetSyntaxColorTheme(
			theme.textColor,
			theme.classColor,
			theme.stringColor,
			theme.symbolColor,
			theme.commentColor,
			theme.numberColor
		);
	}

// state info
	isEdited {
		^this.subclassResponsibility(thisMethod)
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

	string { | rangestart, rangesize = 1 |
		if(rangestart.isNil,{
		^this.text;
		});
		^this.rangeText(rangestart, rangesize);
	}

	string_ { | string, rangestart = -1, rangesize = 1 |
		this.insertTextRange(string, rangestart, rangesize);
	}
	selectedString {
		^this.selectedText
	}


	font_ { | font, rangestart = -1, rangesize=0 |
		this.setFont(font, rangestart, rangesize)
	}

	selectedString_ { | txt |
		this.prinsertText(txt)
	}

	currentLine {
		^this.getSelectedLines(this.selectionStart, 0);
	}
	
	getSelectedLines { | rangestart = -1, rangesize = 0 |
		var start, end, str, max;
		str = this.string;
		max = str.size;
		start = rangestart;
		end = start + rangesize;
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
		this.saveCurrentEnvironment;
		toFrontAction.value(this);
	}

	didResignKey {
		endFrontAction.value(this);
		this.restoreCurrentEnvironment;
	}

	makeWikiPage { | wikiWord, extension=(".rtf"), directory |
		var filename, file, doc, string, dirName;
		directory = directory ? wikiDir;
		filename = directory ++ wikiWord ++ extension;
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
			dirName = wikiWord.dirname;
			if(dirName != ".") {
				dirName = directory ++ dirName;
				"created directory: % \n".postf(dirName);
				unixCmd("mkdir -p" + dirName);
			};
		}
	}

	openWikiPage {
		var selectedText, filename, index, directory;
		var extensions = #[".rtf", ".sc", ".scd", ".txt", "", ".rtfd", ".html"];
		selectedText = this.selectedText;
		index = this.selectionStart;

		this.selectRange(index, 0);

		// refer to local link with round parens
		if(selectedText.first == $( /*)*/ and: {/*(*/ selectedText.last == $) }) {
				selectedText = selectedText[1 .. selectedText.size-2];
				directory = Document.current.path.dirname ++ "/";
		} {
				directory = wikiDir;
		};

		case { selectedText[0] == $* }
		{
			// execute file
			selectedText = selectedText.drop(1);
			extensions.do { |ext|
				filename = directory ++ selectedText ++ ext;
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
				pathMatch(directory ++ selectedText).collect({ |it|it.basename ++ "\n"}).join
			)
		}

		{
			if(index + selectedText.size > this.text.size) { ^this };
			extensions.do { |ext|
				filename = directory ++ selectedText ++ ext;
				if (File.exists(filename)) {
					// open existing wiki page
					this.class.open(filename);
					^this
				}
			};
			// make a new wiki page
			this.makeWikiPage(selectedText, nil, directory);
		};
	}

	mouseUp{ | x, y, modifiers, buttonNumber, clickCount, clickPos |
		mouseUpAction.value(this, x, y, modifiers, buttonNumber, clickCount);		if (wikiBrowse and: { this.linkAtClickPos(clickPos).not }
			and: { this.selectUnderlinedText(clickPos) } ) {
			^this.openWikiPage
		};
	}

	keyDown { | character, modifiers, unicode, keycode |
		this.class.globalKeyDownAction.value(this,character, modifiers, unicode, keycode);
		keyDownAction.value(this,character, modifiers, unicode, keycode);
	}

	keyUp { | character, modifiers, unicode, keycode |
		this.class.globalKeyUpAction.value(this,character, modifiers, unicode, keycode);
		keyUpAction.value(this,character, modifiers, unicode, keycode);
	}

	== { | doc |
		^if(this.path.isNil or: { doc.path.isNil }) { doc === this } {
			this.path == doc.path
		}
	}
	
	hash {
		^(this.path ? this).hash 	
	}
	
	*defaultUsesAutoInOutdent_ {|bool|
		Document.implementationClass.prDefaultUsesAutoInOutdent_(bool)
	}
	
	usesAutoInOutdent_ {|bool|
		this.prUsesAutoInOutdent_(bool)
	}
	
	*prDefaultUsesAutoInOutdent_{|bool|
		this.subclassResponsibility(thisMethod);
	}
	
	prUsesAutoInOutdent_{|bool|
		^this.subclassResponsibility(thisMethod);
	}


// private implementation

	prIsEditable_{ | editable=true |
		^this.subclassResponsibility(thisMethod)
	}
	prSetTitle { | argName |
		^this.subclassResponsibility(thisMethod)
	}
	prGetTitle {
		^this.subclassResponsibility(thisMethod)
	}
	prGetFileName {
		^this.subclassResponsibility(thisMethod)
	}
	prSetFileName { | apath |
		^this.subclassResponsibility(thisMethod)
	}
	prGetBounds { | argBounds |
		^this.subclassResponsibility(thisMethod)
	}

	prSetBounds { | argBounds |
		^this.subclassResponsibility(thisMethod)
	}

	*prSetSyntaxColorTheme{ |textC, classC, stringC, symbolC, commentC, numberC|
		^this.subclassResponsibility(thisMethod);
	}

	// if range is -1 apply to whole doc
	setFont { | font, rangeStart= -1, rangeSize=100 |
		^this.subclassResponsibility(thisMethod)
	}

	setTextColor { | color,  rangeStart = -1, rangeSize = 0 |
		^this.subclassResponsibility(thisMethod)
	}

	text {
		^this.subclassResponsibility(thisMethod)
	}
	selectedText {
		^this.subclassResponsibility(thisMethod)
	}
	selectUnderlinedText { | clickPos |
		^this.subclassResponsibility(thisMethod)
	}

	linkAtClickPos { | clickPos |
		^this.subclassResponsibility(thisMethod)
	}

	rangeText { | rangestart=0, rangesize=1 |
		^this.subclassResponsibility(thisMethod)
	}

	prclose {
		^this.subclassResponsibility(thisMethod)
	}

	closed {
		onClose.value(this); // call user function
		this.restoreCurrentEnvironment;
		allDocuments.remove(this);
		dataptr = nil;
	}

	prinsertText { | dataPtr, txt |
		^this.subclassResponsibility(thisMethod)
	}
	insertTextRange { | string, rangestart, rangesize |
		^this.subclassResponsibility(thisMethod)
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
	*prnumberOfOpen {
		^this.subclassResponsibility(thisMethod)
	}
	*numberOfOpen {
		thisProcess.platform.when(\_NumberOfOpenTextWindows) {
			^this.prnumberOfOpen
		} { ^allDocuments.size };
		^0
	}

	*newFromIndex { | idx |
		^super.new.initByIndex(idx)
	}
	initByIndex { | idx |
		//allDocuments = allDocuments.add(this);
		var doc;
		doc = this.prinitByIndex(idx);
		if(doc.isNil,{^nil});
		this.prAdd;
	}
	prinitByIndex { | idx |
		^this.subclassResponsibility(thisMethod)
	}

	//this is called from the menu: open, new
	*prGetLast {
		^Document.implementationClass.prBasicNew.initLast
	}

	initLast {
		^this.subclassResponsibility(thisMethod)
	}

	prGetLastIndex {
		^this.subclassResponsibility(thisMethod)
	}

	// private open
	initFromPath { | path, selectionStart, selectionLength |
		var stpath;
	//	path = apath;
		stpath = this.class.standardizePath(path);
		this.propen(stpath, selectionStart, selectionLength);
		if(dataptr.isNil,{
			this.class.allDocuments.do{ |d|
					if(d.path == stpath.absolutePath){
						^d
					}
				};
			^nil
		});
		this.background_(Color.white);
		^this.prAdd;
	}
	propen { | path, selectionStart=0, selectionLength=0 |
		^this.subclassResponsibility(thisMethod)
	}

	// private newTextWindow
	initByString{ | argTitle, str, makeListener |

		this.prinitByString(argTitle, str, makeListener);
		this.background_(Color.white);
		if(dataptr.isNil,{^nil});
		this.prAdd;
		this.title = argTitle;

	}
	prinitByString { | title, str, makeListener |
		^this.subclassResponsibility(thisMethod)
	}

	// other private
	// if -1 whole doc

	prSetBackgroundColor { | color |
		^this.subclassResponsibility(thisMethod)
	}
	prGetBackgroundColor { | color |
		^this.subclassResponsibility(thisMethod)
	}
	prSetSelectedBackgroundColor { | color |
		^this.subclassResponsibility(thisMethod);
	}
	prGetSelectedBackgroundColor { | color |
		^this.subclassResponsibility(thisMethod);
	}
	selectedRangeLocation {
		^this.subclassResponsibility(thisMethod)
	}
	selectedRangeSize {
		^this.subclassResponsibility(thisMethod)
	}

	prSelectLine { | line |
		^this.subclassResponsibility(thisMethod)
	}

	*prGetIndexOfListener {
		^this.subclassResponsibility(thisMethod)
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


// Environment handling  Document with its own envir must set and restore currentEnvironment on entry and exit.
// Requires alteration of *open, *new, closed, didBecomeKey, and didResignKey

	envir_ { | ev |
		envir = ev;
		if (this.class.current == this) {
			if (savedEnvir.isNil) {
				this.saveCurrentEnvironment
			}
		}
	}

	restoreCurrentEnvironment {
		if (envir.notNil) { currentEnvironment = savedEnvir };
	}

	saveCurrentEnvironment {
		if (envir.notNil) {
			savedEnvir = currentEnvironment;
			currentEnvironment = envir;
		}
	}

	*prBasicNew {
		^super.new
	}

}


