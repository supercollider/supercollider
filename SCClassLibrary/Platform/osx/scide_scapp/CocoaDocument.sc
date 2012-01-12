CocoaDocument : Document {
	classvar <defaultFont;

	*initClass{
		Document.implementationClass = CocoaDocument;
	}

	*startup {
		var post;
		super.startup;
		fork({
			0.2.wait;
			post = this.listener;
			if(post.notNil) { post.name_(" post ") }
		}, AppClock);
		this.setTheme('default');
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

	*postColor_{ arg color;
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
