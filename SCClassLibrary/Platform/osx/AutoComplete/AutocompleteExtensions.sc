// class extensions needed for autocomplete

+ Document {
	getIdentifierCoordFromEnd { |endPos|
		var origpos, pos, chartemp, id;
		pos = (origpos = endPos ? this.selectionStart) - 1;
		{ (pos >= 0) and:
			{ (chartemp = this.string(pos, 1)[0]).isAlphaNum or: (chartemp == $_) } }
			.while({
				pos = pos - 1;
			});
		^[pos+1, origpos-pos-1]
	}

	*allowAutoComp { ^true }		// maybe you want to turn off the IDE features

	*autoCompAll {
		this.globalKeyDownAction = this.autoCompleteKeyAction;
	}

	*autoComplete {
		^Document.new.autoComplete;
	}

	autoComplete {	// enable ide for this document
		Document.allowAutoComp.if({
			keyDownAction = Document.autoCompleteKeyAction;
		});
	}

	*autoCompleteKeyAction {
		^{ |doc, char, modifiers, keycode|
			var	start, size;
			char = char.ascii;
			(char == 40 and: (modifiers.bitAnd(131072) > 0)).if({
				#start, size = doc.getIdentifierCoordFromEnd;
				AutoCompMethodBrowser(start, size, doc)
			}, {
				(char == 46).if({
					#start, size = doc.getIdentifierCoordFromEnd;
					AutoCompClassBrowser(start, size, doc)
				}, {
					(keycode == 15 and: (modifiers.bitAnd(262144) > 0)).if({
						#start, size = doc.getIdentifierCoordFromEnd;
						AutoCompClassSearch(start, size, doc)
					});
				});
			});
		};
	}

	*openFileAutoComplete {	arg path;		// open a file by path
							// can use wildcards
		path.pathMatch.do({ |p|
			Document.open(p).autoComplete;
		});
	}

	*openAutoComplete {	// open a file by opendialog
		File.openDialog("Choose the codefile to open", { |path|
			this.openFileAutoComplete(path);
		});
	}

}

+ Class {
	metaclass {
		this.isMetaClass.if({ ^this },{ ^this.class });
	}
}

+ Method {
	argList { arg skipThis = true;
		var str;
		(skipThis and: (argNames.size <= 1)).not.if({ str = "(" }, { str = "" });
		argNames.do({ |a, i|
				// i > 0 if including "this", i > 1 if not
			(i > (skipThis.binaryValue)).if({ str = str ++ ", " });
			(skipThis.not and: (i == 0)).if({ str = str ++ "a_" ++ ownerClass.name });
			(i > 0).if({ str = str ++ a });
		});
		^(str.size > 0).if({ str ++ ")" }, { str });
	}
}

+ Nil {
	isAlphaNum { ^false }	// error protection in getIdCoords
	ascii {}
}
