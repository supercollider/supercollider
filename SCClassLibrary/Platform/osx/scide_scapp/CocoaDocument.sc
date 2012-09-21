CocoaDocument : Document {
	classvar <defaultFont;
	classvar <postColor;
	classvar <theme, <themes, <wikiDir="";
	classvar <>wikiBrowse = true;

	*initClass{
		Document.implementationClass = CocoaDocument;
	}

	*wikiDir_ { | path |
		path = path.standardizePath;
		if(path == "") {wikiDir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				wikiDir = path ++ "/"
			}
		}
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
				dirName.mkdir;
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
			selectedText[1 .. selectedText.size-2].help
		}
		{ selectedText.containsStringAt(0, "http://")
			or: { selectedText.containsStringAt(0, "file://") } }
		{
			// open URL
			openOS(selectedText)
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
		mouseUpAction.value(this, x, y, modifiers, buttonNumber, clickCount);
		if (wikiBrowse and: { this.linkAtClickPos(clickPos).not }
			and: { this.selectUnderlinedText(clickPos) } ) {
			^this.openWikiPage
		};
	}

	*startup {
		var post;
		super.startup;

		postColor = Color.black;
		themes = (
			default: (
				classColor: Color(0, 0, 0.75, 1),
				textColor: Color(0, 0, 0, 1),
				stringColor: Color(0.375, 0.375, 0.375, 1),
				commentColor: Color(0.75, 0, 0, 1),
				symbolColor: Color(0, 0.45, 0, 1),
				numberColor: Color(0, 0, 0, 1),
				specialValsColor: Color(0.2, 0.2, 0.75, 1), // true false nil inf
				specialVarsColor: Color(0.4, 0.4, 0.75, 1), // super, thisProcess
				declColor: Color(0, 0, 1, 1), // var, const, args
				puncColor: Color(0, 0, 0, 1),
				environColor: Color(1.0, 0.4, 0, 1)
				)
			);
		theme = themes.default;

		fork({
			0.2.wait;
			post = this.listener;
			if(post.notNil) { post.name_(" post ") }
		}, AppClock);
		this.setTheme('default');
	}

	*setTheme { | themeName |
		theme = themes[themeName];
		if(theme.proto.isNil) {
			theme = theme.copy.parent_(themes[\default]);
		};
		thisProcess.platform.writeClientCSS;
		this.prSetSyntaxColorTheme(
			theme.textColor,
			theme.classColor,
			theme.stringColor,
			theme.symbolColor,
			theme.commentColor,
			theme.numberColor,
			theme.specialValsColor,
			theme.specialVarsColor,
			theme.declColor,
			theme.puncColor,
			theme.environColor
		);
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
		^Document.implementationClass.prPostColor_(col);
	}

	stringColor_ { | color, rangeStart = -1, rangeSize = 0 |
		stringColor = color;
		this.setTextColor(color,rangeStart, rangeSize);
	}


	*new { arg title="Untitled", string="", makeListener=false;
		^super.prBasicNew.initByString(title, string.asString, makeListener);
	}

	*defaultFont_{|font|
		font = font.asSCFont;
		CocoaDocument.prSetDefaultFont(font);
		defaultFont = font;
	}

	*prSetDefaultFont{|font|
		_TextWindow_SetDefaultFont
	}

	*prSetSyntaxColorTheme {|textC, classC, stringC, symbolC, commentC, numberC, specialValsC, specialVarsC, declC, puncC, environC|
		_TextWindow_SetSyntaxColorTheme
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

	balanceParens { arg levels = 1;
		var prev = this.selectionStart, next;
		levels.do {
			this.prBalanceParens;
			prev = next;
			next = this.selectionStart;
			if(prev == next) { ^this };
		}
	}

	*prPostColor_{ arg color;
		_PostWindow_SetTextColor
	}

// state info
	isEdited {
		_TextWindow_IsEdited
	}



//private-----------------------------------

	*prDefaultUsesAutoInOutdent_{|bool|
		_TextWindow_DefaultUsesAutoInOutdent
	}

	prUsesAutoInOutdent_{|bool|
		_TextWindow_UsesAutoInOutdent
	}

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
	prBalanceParens {
		_TextWindow_BalanceParens
	}


	//if range is -1 apply to whole doc
	setFont {arg font, rangeStart= -1, rangeSize=100;
		this.prSetFont(font.asSCFont, rangeStart, rangeSize);
	}

	prSetFont { arg font, rangeStart, rangeSize;
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

	linkAtClickPos { arg clickPos;
		_TextWindow_LinkAtClickPos
		^false
	}

	rangeText { arg rangestart=0, rangesize=1;
		_TextWindow_TextWithRange
	}

	prclose {
		_TextWindow_Close
	}

	prinsertText { arg dataPtr, txt;
	 	_TextWindow_InsertText
	}
	insertTextRange { arg string, rangestart, rangesize;
		_TextWindow_InsertTextInRange
		^this.primitiveFailed
	}





	*prnumberOfOpen {
		_NumberOfOpenTextWindows
	}
	prinitByIndex { arg idx;
		_TextWindow_GetByIndex
	}
	prGetLastIndex {
		_TextWindow_GetLastIndex
	}


	propen { arg path, selectionStart=0, selectionLength=0;
		_OpenTextFile
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
	prSetSelectedBackgroundColor { arg color;
		_TextWindow_SetSelectedBackgroundColor
	}
	prGetSelectedBackgroundColor{ arg color;
		_TextWindow_GetSelectedBackgroundColor
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

	initLast {
		this.prGetLastIndex;
		if(dataptr.isNil,{^nil});
		this.background_(Color.white);
		this.prAdd;
	}
}
