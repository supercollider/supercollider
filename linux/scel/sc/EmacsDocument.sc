EmacsDocument : Document
{
	var <>isEdited=false;

	*initClass {
		Class.initClassTree(EmacsInterface);
		EmacsInterface
		.put(\documentNew, { | name, path |
			this.prNewFromLisp(name, path);
			nil
		})
		.put(\documentClosed, { | name |
			this.documentDo(name, { | doc |
				doc.closed;
			});
			nil
		})
		.put(\documentEdited, { | name, flag |
			this.documentDo(name, { | doc |
				doc.isEdited = (flag === true);
			});
		})
		.put(\documentBecomeKey, { | name, flag |
			this.documentDo(name, { | doc |
				if (flag === true) {
					doc.didBecomeKey;
				}{
					doc.didResignKey;
				}
			});
		})
	}

	*listenerName { ^"*SCLang*" }

	*findDocument { | name |
		^allDocuments.detect({ | doc | doc.dataptr == name })
	}
	*documentDo { | name, function |
		var doc;
		doc = this.findDocument(name);
		^if (doc.notNil) {
			function.value(doc);
		}
	}
	*documentDoMsg { | name, selector ... args |
		var doc;
		doc = this.findDocument(name);
		^if (doc.notNil) {
			doc.performList(selector, args);
		}
	}

	// lisp support
	storeLispOn { | stream |
		dataptr.storeLispOn(stream)
	}

	//document setup	
	background_ {arg color, rangestart= -1, rangesize = 0;
	}	
	stringColor_ {arg color, rangeStart = -1, rangeSize = 0;
	}

	//interaction:
	front {
		//_TextWindow_ToFront
		Emacs.sendToLisp(\_documentSwitchTo, this);
	}
	
	unfocusedFront {
		//_TextWindow_UnfocusedFront
		Emacs.sendToLisp(\_documentPopTo, this);
	}
	
	syntaxColorize {
		//_TextWindow_SyntaxColorize
	}
	
	selectRange { arg start=0, length=0;
		//_TextWindow_SelectRange
	}
	prisEditable_{ arg editable=true;
		//_TextWindow_SetEditable
	}
	removeUndo{
		//_TextWindow_RemoveUndo
	}
	
	string {arg rangestart, rangesize = 1;
// 		if(rangestart.isNil,{
// 		^this.text;
// 		});
// 		^this.rangeText(rangestart, rangesize);
		^""
	}
	
	currentLine {
// 		var start, end, str, max;
// 		str = this.string;
// 		max = str.size;
// 		end = start = this.selectionStart;
// 		while { 
// 			str[start] !== Char.nl and: { start >= 0 }
// 		} { start = start - 1 };
// 		while { 
// 			str[end] !== Char.nl and: { end < max }
// 		} { end = end + 1 };
// 		^str.copyRange(start + 1, end);
		^""
	}
	
	// PRIVATE
	*prNewFromLisp { | name, path |
		^super.new.prInitFromLisp(name, path);
	}
	prInitFromLisp { | argName, argPath |
		dataptr = argName;
		path = argPath;
		this.prisListener(argName == this.class.listenerName);
		if (this.isListener) {
			thelistener = this;
		};
		this.prAdd;
	}
	prRemove {
		allDocuments.remove(this);
		dataptr = nil;
	}
	prSetName { arg argName;
		this.prRemove;
		Emacs.sendToLisp(\_documentSetName, argName, {
			dataptr = argName;
			this.prAdd
		});
	}
	prGetBounds { arg argBounds;
		// return like what, width x height in characters?
		^argBounds
	}
	prSetBounds { arg argBounds;
	}

	// if range is -1 apply to whole doc
	setFont {arg font, rangeStart= -1, rangeSize=100;
	}
	setTextColor { arg color,  rangeStart = -1, rangeSize = 0;
	}
	text {
		^""
	}
	selectedText {
		^""
	}	
	rangeText { arg rangestart=0, rangesize=1; 
		^""
	}
	prclose {
		Emacs.sendToLisp(\_documentClose, this);
	}
	prinsertText { arg dataptr, txt;
	}
	insertTextRange { arg string, rangestart, rangesize;
	}
	initByIndex { arg idx;
		^this.shouldNotImplement
	}
	prinitByIndex { arg idx;
		^this.shouldNotImplement
	}	
	initLast {
		^this.shouldNotImplement
	}
	prGetLastIndex {
		^this.shouldNotImplement
	}
	//private open
	initFromPath { arg apath, selectionStart, selectionLength;
// 		var stpath;
// 		path = apath;
// 		stpath = this.class.standardizePath(path);
// 		this.propen(stpath, selectionStart, selectionLength);
// 		if(dataptr.isNil,{ this = nil; ^nil});
// 		isListener = false;
// 		this.prAdd;
	}
	propen { arg path, selectionStart=0, selectionLength=0;
// 		_OpenTextFile
	}
	//private newTextWindow
	initByString{arg argTitle, str, makeListener;
// 		title = argTitle;
// 		if(makeListener, {
// 			allDocuments.do({arg doc; doc.prisListener(false)})
// 			});
// 		isListener = makeListener;
// 		thelistener = this;
// 		this.prinitByString(title, str, makeListener);
// 		if(dataptr.isNil,{ this = nil; ^nil});
// 		this.prAdd;	
	}
	prinitByString { arg title, str, makeListener;
		if (makeListener) {
			// there's only one post buffer
			^this.class.listener
		};
	}
	prgetTitle {
		^dataptr ? "Untitled"
	}
	setBackgroundColor { arg color;
	}	
	selectedRangeLocation {
		^0
	}
	selectedRangeSize {
		^0
	}
	prselectLine { arg line;
		^""
	}
}