// copyright 2007 Marije Baalman (nescivi AT gmail DOT com)
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

ScelDocument : Document{
	var <thisdoc;
	var cFuncs;
	var checkCurrent;
	var <envir;
	var title_p, path_p;
	var <>currentString;

	*new{ | title = "Untitled", string = "", makeListener = false, toFront=true |
		//		"ScelDocument.new".postln;
		^super.prBasicNew.init( title, string, makeListener, toFront );
	}

	*open{  | path, selectionStart = 0, selectionLength = 0, toFront=true |
		^super.prBasicNew.initFromPath( path, selectionStart, selectionLength, toFront );
	}

	*newFromEmacs{ |doc|
		^this.prBasicNew.prinitFromEmacs( doc );
	}

	prinitFromEmacs{ |doc|
		thisdoc = doc;
		thisdoc.sceld = this;
		checkCurrent = { |doc| if ( EmacsDocument.current === doc, { this.didBecomeKey } ); };
		checkCurrent.value( doc );
		^this;
	}

	initFromPath{ | path, selectionStart = 0, selectionLength = 0, toFront=true|
		checkCurrent = { |doc| if ( EmacsDocument.current === doc, { this.didBecomeKey } ); };
		cFuncs = [checkCurrent];
		path_p = path;
		title_p = path;
		EmacsDocument.prNewFromPath(path, selectionStart, selectionLength, { |doc| thisdoc = doc; thisdoc.sceld = this; cFuncs.do{ |it| it.value(doc); } } );
		if ( toFront, { this.front } );
		^this
	}

	init{ |title, string, makeListener, toFront|
		//		"ScelDocument.init".postln;
		checkCurrent = { |doc| if ( EmacsDocument.current === doc, { this.didBecomeKey } ); };
		cFuncs = [checkCurrent];
		title_p = title;
		EmacsDocument.prNewFromString(title, string, makeListener, { |doc| thisdoc = doc; thisdoc.sceld = this; cFuncs.do{ |it| it.value(doc)} });
		if ( toFront, { this.front } );
		^this
	}

	string_ { | argName, rangestart = -1, rangesize = 1 |
		if ( thisdoc.notNil, {
			thisdoc.string_( argName, rangestart, rangesize )
		},{
			cFuncs = cFuncs ++ { this.string_( argName ) };
		});
	}

	title_ { | argName, completFunc |
		if ( thisdoc.notNil, {
			thisdoc.title_( argName, completFunc )
		},{
			cFuncs = cFuncs ++ { this.title_( argName, completFunc ) };
		});
	}

	title{
		if ( thisdoc.notNil, {
			^thisdoc.title;
		},{
			^("***"++title_p++"***")
		});
	}

	// printing
	printOn { | stream |
		super.printOn(stream);
		stream << $( << this.title << $);
	}

	prGetFileName {
		if ( thisdoc.notNil, {
			^thisdoc.path;
		},{
			^path_p;
		});
	}
	prSetFileName { | argPath |
		"sceldoc.prSetFileName".postln;
		if ( thisdoc.notNil, {
			thisdoc.prSetFileName( argPath );
		},{
			cFuncs = cFuncs ++ { this.prSetFileName( argPath ) };
		});
	}

	prSetFileName_ { | argPath |
		"sceldoc.prSetFileName_".postln;
		path_p = argPath;
	}

	path_{ |path|
		"sceldoc.path".postln;
		this.prSetFileName( path );
		//		if ( thisdoc.notNil, { thisdoc.path_( path ) },{ completionFuncs = completionFuncs ++ { this.path_( path ) };  });
		//		^this
	}

	front {
		if ( thisdoc.notNil, {
			thisdoc.front
		},{
			cFuncs = cFuncs ++ { this.front };
		});
	}

	unfocusedFront {
		if ( thisdoc.notNil, {
			thisdoc.unfocusedFront;
		},{
			cFuncs = cFuncs ++ { this.unfocusedFront };
		});
	}
	syntaxColorize {
		if ( thisdoc.notNil, {
			thisdoc.syntaxColorize;
		},{
			cFuncs = cFuncs ++ { this.syntaxColorize };
		});
	}
	prisEditable_{ | flag = true |
		if ( thisdoc.notNil, {
			thisdoc.prisEditable_( flag );
		},{
			cFuncs = cFuncs ++ { this.prisEditable_( flag ) };
		});
		editable = flag;
	}

	removeUndo{
		if ( thisdoc.notNil, {
			thisdoc.removeUndo
		},{
			cFuncs = cFuncs ++ { this.removeUndo };
		});
	}

	envir_ { | environment |
		envir = environment;
		if (this === current) {
			envir.push;
		}
	}

	didBecomeKey {
		if (envir.notNil) {
			envir.push;
		};
		super.didBecomeKey;
		EmacsDocument.current = this;
	}

	didResignKey {
		if (envir === currentEnvironment) {
			envir.pop;
		};
		super.didResignKey;
	}

	//	envir_ { | environment | thisdoc.envir_( environment ) }
	//	didBecomeKey { thisdoc.didBecomeKey }
	//	didResignKey { thisdoc.didResignKey }

	closed {
		thisdoc.prRemove;
		onClose.value(this); // call user function
		//		allDocuments.remove(this);
		//		dataptr = nil;
	}

	isEdited {
		if ( thisdoc.notNil, {
			^thisdoc.isEdited
		},{
			^false;
		});
	}
	//	isFront { thisdoc.isFront }
	editable_{arg abool=true; this.prisEditable_( abool ) }

	/* should maybe be this:
	path{
		if ( thisdoc.notNil, {
			^^thisdoc.prGetFileName;
		},{
			^path_p;
		});
	*/

	path{^thisdoc.prGetFileName }

	*addToList{ |doc|
		var key, sceld;
		//		"adding to List".postln;
		key = allDocuments.detectIndex( { |it| it.thisdoc === doc } );
		if ( key.isNil,
			{
				sceld = ScelDocument.newFromEmacs( doc );
				allDocuments = allDocuments.add(sceld);
				initAction.value(sceld);
			});
	}
	*removeFromList{ |doc|
		var toremove;
		toremove = allDocuments.detectIndex( { |it| it.thisdoc === doc }  );
		if ( toremove.notNil,
			{
				allDocuments.removeAt(toremove);
			});
	}

	prclose {
		if ( thisdoc.notNil,{
			thisdoc.prclose
		},{
			cFuncs = cFuncs ++ { this.prclose };
		});
	}

	string {arg rangestart, rangesize = 1;
		//		var cond;
		currentString = nil;
		thisdoc.string( rangestart, { |v| currentString = v }, rangesize );
//		cond = Condition.new( { currentString.notNil } );
//		cond.wait;
		//		while ( { currentString.isNil }, {"wait for string".postln;} );
		"Asynchronous: retrieve the result with .currentString".postln;
		^currentString;
	}

	currentLine {
		//		var cond;
		currentString = nil;
		thisdoc.currentLine( { |v| currentString = v } );
//		cond = Condition.new( { currentString.notNil } );
//		cond.wait;
		//		while ( { currentString.isNil }, {"wait for string".postln;} );
		"Asynchronous: retrieve the result with .currentString".postln;
		^currentString;
	}

	currentWord {
		//		var cond;
		currentString = nil;
		thisdoc.currentWord( { |v| currentString = v } );
//		cond = Condition.new( { currentString.notNil } );
//		cond.wait;
		//		while ( { currentString.isNil }, {"wait for string".postln;} );
		"Asynchronous: retrieve the result with .currentString".postln;
		^currentString;
	}

	currentBlock {
		//		var cond;
		currentString = nil;
		thisdoc.currentBlock( { |v| currentString = v } );
//		cond = Condition.new( { currentString.notNil } );
//		cond.wait;
		//		while ( { currentString.isNil }, {"wait for string".postln;} );
		"Asynchronous: retrieve the result with .currentString".postln;
		^currentString;
	}


	text {
		^this.string;
	}
	rangeText { arg rangestart=0, rangesize=1;
		^this.string( rangestart, rangesize );
	}

	dataptr { ^thisdoc.tryPerform(\dataptr) }

	// not implemented:
	selectRange { arg start=0, length=0; }
	background_ {arg color, rangestart= -1, rangesize = 0;
	}
	stringColor_ {arg color, rangeStart = -1, rangeSize = 0;
	}

	prGetBounds { | bounds | ^bounds }
	prSetBounds { }
	setFont { }
	setTextColor { }
	selectedText {
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

	bounds_{
	}

	*current {
		var cur = EmacsDocument.current;
		if ( cur.isNil ){
			^cur;
		}{
			^cur.sceld;
		}
	}

	*prGetIndexOfListener{
		^this.allDocuments.detectIndex( { |doc| doc.title == "*SCLang:PostBuffer*" } );
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
	*postColor_{
	}
}