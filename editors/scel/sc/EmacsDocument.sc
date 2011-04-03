// copyright 2003 stefan kersten <steve@k-hornz.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
// USA

EmacsDocument
// : Document
{
	classvar documentMap, <>current;
	var <>sceld;
	var title, path;
	var <dataptr;
	var <isEdited, <isListener, <envir;

	*initClass {
		documentMap = IdentityDictionary.new;
		Class.initClassTree(EmacsInterface);
		Class.initClassTree(ScelDocument);
		EmacsInterface
		.put(\documentNew, { | id, makeEnvir |
			var doc;
			if (documentMap.includesKey(id).not) {
				//				[\documentNew, id, makeEnvir].postln;
				doc = this.prBasicNew.prInitFromLisp(id);
				// is this necessary?? when is this the case??
				if (makeEnvir.notNil) {
					doc.envir = Environment.new;
				};
			};
			nil
		})
		.put(\documentClosed, { | id |
			this.documentDo(id, { | doc |
// 				[\documentClosed, doc].postln;
				doc.sceld.closed;
			});
			nil
		})
		.put(\documentSetCurrent, { | id |
			if (current.notNil) {
// 				[\didResignKey, current].postln;
				current.didResignKey;
			};
			if (id.notNil) {
				this.documentDo(id, { | doc |
// 					[\didBecomeKey, doc].postln;
					doc.didBecomeKey;
				});
			};
			nil
		})
		.put(\documentSetProperty, { | id, msg, value |
			this.documentDo(id, { | doc |
// 				[\documentSetProperty, doc, msg, value].postln;
				doc.perform(msg, value);
			});
			nil
		});
	}

	*documentDo { | id, function |
		var doc;
		doc = documentMap.at(id);
		^if (doc.notNil) {
			function.value(doc);
		}
	}
	*documentDoMsg { | id, selector ... args |
		var doc;
		doc = documentMap.at(id);
		^if (doc.notNil) {
			doc.performList(selector, args);
		}
	}

	// lisp support
	storeLispOn { | stream |
		dataptr.storeLispOn(stream)
	}

	// printing
	printOn { | stream |
		super.printOn(stream);
		stream << $( << this.title << $);
	}

	//document setup
	title_ { | argName, completionFunc |
		Emacs.sendToLisp(\_documentRename, [this, argName], {
			completionFunc.value(this);
		});
	}

	title{
		^title;
	}

	background_ {arg color, rangestart= -1, rangesize = 0;
	}
	stringColor_ {arg color, rangeStart = -1, rangeSize = 0;
	}

	//interaction:
	front {
		Emacs.sendToLisp(\_documentSwitchTo, this);
	}

	unfocusedFront {
		Emacs.sendToLisp(\_documentPopTo, this);
	}

	syntaxColorize {
		Emacs.sendToLisp(\_documentSyntaxColorize, this);
	}

	selectRange { arg start=0, length=0;
		//_TextWindow_SelectRange
	}
	prisEditable_{ | flag = true |
		Emacs.sendToLisp(\_documentSetEditable, [this, flag]);
	}
	removeUndo{
		Emacs.sendToLisp(\_documentRemoveUndo, this);
	}

	string{ arg rangestart, returnFunc, rangesize = 1;
		var rangeend, resultString;
		if ( rangestart.isNil,{
			rangestart = '(point-min)';
			rangeend = '(point-max)';
		},{
			rangeend = rangestart + rangesize;
		});
		Emacs.evalLispExpression(['with-current-buffer', title, [ 'buffer-substring-no-properties', rangestart, rangeend ]].asLispString;, { |result| returnFunc.value( result ); } );
		^nil;
	}

	string_{|string, rangestart = -1, rangesize = 1|
		Emacs.sendToLisp(\_documentPutString, [this, string]);
	}

	currentLine { |returnFunc|
		Emacs.evalLispExpression(['sclang-line-at-point'].asLispString, { |result| returnFunc.value( result ) }  );
		//		['with-current-buffer', title, ['thing-at-point', '\'line'] ].asLispString, { |result| returnFunc.value( result ) } )
		^nil;

		// '(set-text-properties start end nil)' will remove text properties somehow?
	}

	currentBlock { |returnFunc|
		Emacs.evalLispExpression(['sclang-defun-at-point'].asLispString, { |result| returnFunc.value( result ) }  );
		^nil;
	}

	currentWord { |returnFunc|
		Emacs.evalLispExpression(['with-current-buffer', title, ['current-word'] ].asLispString, { |result| returnFunc.value( result ) } )
		^nil;
	}

	// environment support
	/*	envir_ { | environment |
		envir = environment;
		if (this === current) {
			envir.push;
		}
	}
	*/

	didBecomeKey {
		if (envir.notNil) {
			envir.push;
		};
		current = this;
	}

	didResignKey {
		if (envir === currentEnvironment) {
			envir.pop;
		};
		if ( current === this, { current = nil } );
		//super.didResignKey;
	}


	// PRIVATE
	*prNewFromPath { | argPath, selectionStart, selectionLength, completionFunc |
		argPath = Document.standardizePath(argPath);
		Emacs.sendToLisp(
			\_documentOpen,
			[argPath, selectionStart + 1, selectionLength],
			{ | id |
				if (id.isNil) {
			 		"Couldn't create document".warn;
				}{
					this.documentDo(id, completionFunc);
				}
			});
	}
	*prNewFromString { | name, str, makeListener, completionFunc |
		Emacs.sendToLisp(
		 	\_documentNew,
			[name, str, makeListener],
			{ | id |
				if (id.isNil) {
					"Couldn't create document".warn;
				}{
					this.documentDo(id, completionFunc);
				}
			});
	}
	prInitFromLisp { | id |
		dataptr = id;
		this.prAdd;
	}
	prclose {
		if (dataptr.notNil) {
			Emacs.sendToLisp(\_documentClose, this);
		}
	}
	prAdd {
		ScelDocument.addToList( this );
		//		allDocuments = allDocuments.add(this);
		documentMap.put(dataptr, this);
		//initAction.value(this);
	}

	prRemove {
		ScelDocument.removeFromList( this );
		//	allDocuments.remove(this);
		documentMap.removeAt(dataptr);
		dataptr = nil;
	}

	prGetTitle {
		^title
	}
	prSetTitle { | argTitle |
		title = argTitle;
	}


	prGetFileName {
		^path
	}
	prSetFileName { | argPath |
		path = argPath;
		if (path.notNil) {
			path = Document.standardizePath(path);
		}
	}

	prSetIsListener { | flag |
		isListener = flag.notNil;
	}
	prSetEditable { | flag |
		//		sceld.editable = flag.notNil;
	}
	prSetEdited { | flag |
		isEdited = flag.notNil;
	}

	*prBasicNew { ^super.new }

	// unimplemented methods
/*
	prGetBounds { | bounds | ^bounds }
	prSetBounds { }
	setFont { }
	setTextColor { }
	text {
		^""
	}
	selectedText {
		^""
	}
	rangeText { arg rangestart=0, rangesize=1;
		^""
	}
	prinsertText { arg dataptr, txt;
	}
	insertTextRange { arg string, rangestart, rangesize;
	}
	setBackgroundColor { }
	selectedRangeLocation {
		^0
	}
	selectedRangeSize {
		^0
	}
	prselectLine { arg line;
	}

	// invalid methods
	initByIndex {
		^this.shouldNotImplement(thisMethod)
	}
	prinitByIndex {
		^this.shouldNotImplement(thisMethod)
	}
	initLast {
		^this.shouldNotImplement(thisMethod)
	}
	prGetLastIndex {
		^this.shouldNotImplement(thisMethod)
		}
*/
}

// EOF