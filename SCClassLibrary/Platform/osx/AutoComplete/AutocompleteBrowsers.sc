AutoCompMethodBrowser {
	classvar	<>wWidth = 400, <>wHeight = 360;
	classvar	<methodExclusions, <classExclusions;

	classvar	<w, textField, listView,		// gui objects
		selector,			// string typed by the user to match to classes or methods
		masterList,		// a list of all methods with that selector
		reducedList,		// only the ones displayed
		skipThis,			// flag: do (this, xxx) or (xxx) in argList
		dropMeta,			// flag: display Meta_ for metaclasses?
		doc,				// document from which this was created
		start, size;		// start and length of identifier in document

	classvar	overwriteOnCancel = false;

	*initClass {
		methodExclusions = methodExclusions.asArray;
	}

		// prevent certain method selector strings from being gui'ed during typing
		// assumes that selectors are spelled correctly!
		// startup.rtf should include AutoCompMethodBrowser.exclude([\value, \if, ...]);
	*exclude { |selectorArray|
		methodExclusions = methodExclusions ++ selectorArray.collect({ |sel| sel.asSymbol });
	}

	*new { arg start, size, doc;
			// may only open a browser if there isn't one open already
		(w.isNil and: { this.newCondition(start, size, doc) }).if({
			^this.init(start, size, doc)
		}, {
			w.notNil.if({ w.front });
			^nil
		});
	}

	*newCondition { arg start, size, doc;
		var	selector;
		selector = doc.string(start, size).asSymbol;
		^Document.allowAutoComp and: { methodExclusions.includes(selector).not }
	}

	*init { arg argStart, argSize, argDoc;
		var	displaySel, temp, initString;
		skipThis = dropMeta = false;
		selector = argDoc.string(argStart, argSize);
			// if no string, abort
		(selector.size == 0).if({ ^nil });
			// if it's part of a class name,
		(selector[0] >= $A and: { selector[0] <= $Z }).if({
			AutoCompClassBrowser.classExclusions.includes(selector.asSymbol.asClass).not.if({
					// identify classes containing that string
				masterList = Class.allClasses.select({ |cl|
					cl.isMetaClass and: { cl.name.asString.containsi(selector) }
				}).collect({ |cl|	// then grab their *new methods
					[cl, cl.findRespondingMethodFor(\new), cl.name]  // cl.name used for sorting
				}).reject({ |item| item[1].isNil });
				initString = selector;
				displaySel = "new";
				skipThis = dropMeta = true;
				overwriteOnCancel = true;
			});
		}, {
			masterList = IdentitySet.new;
			Class.allClasses.do({ |class|
				class.methods.do({ |method|
					method.name.asString.containsi(selector).if({
						masterList.add([class, method,
							class.name ++ "-" ++ method.name
						]);
					});
				});
			});
			initString = "";
			displaySel = selector;
			overwriteOnCancel = false;
		});
		(masterList.size > 0).if({
				// this sort will sort both of them (desired)
				// [0] is the ownerclass, [1] the method
			masterList = masterList.asArray.sort({ |a, b| a[2] < b[2] });
				// if previous char is a ., then class should not be displayed in argList
			(argStart > 0 and: { argDoc.string(argStart-1, 1)[0] == $. }).if({ skipThis = true });
			doc = argDoc;
			start = argStart;
			size = argSize;
			this.prInit("." ++ displaySel);
			textField.string_(initString);
			this.restrictList;
		}, {
			^nil
		});
	}

	*free { |finished = false|
		var	string;
			// if window is nil, isclosed should be true
			// close the window only if it isn't closed
		(w.tryPerform(\isClosed) ? true).not.if({
				// if there's typing in the text box and no possible autocomplete,
				// add it into the document
			((string = textField.string).size > 0 and: { finished.not }).if({
					// does the string start with the selector?
					// size var is size of selector
				(string[0..size-1] == selector).if({
						// if so, drop it so the rest can go in the document
					string = string[size..];
				}, {
						// if not, and the selector is a class, we need to drop it from doc
					overwriteOnCancel.if({
						doc.selectRange(start, size+1)
					});
				});
				doc.selectedString_(string);
			});
			w.close;
		});
			// garbage; also, w = nil allows next browser to succeed
		w = masterList = reducedList = nil;
	}

	*finish {
		var	selectStart, selectSize, str;
			// select the right text in the doc and replace with method template
		(reducedList.size > 0).if({
			doc.selectRange(start, size+1)	// must replace open paren which size doesn't include
				.selectedString_(str = this.finishString(reducedList[listView.value]));
			#selectStart, selectSize = this.finalSelection(str);
			doc.selectRange(selectStart, selectSize);	// reposition cursor
			textField.string_("");  // .free will do something bad if I don't clear this
		});
		this.free(true);
	}

	*finalSelection { |str|
		var  openParen, closeParen;
		(openParen = str.detectIndex({ |ch| ch.ascii == 40 })).isNil.if({
			^[start + str.size, 0]
		}, {
			closeParen = str.detectIndex({ |ch| ch.ascii == 41 });
			^[openParen + 1 + start, closeParen-openParen-1]
		});
	}

	*finishString { |meth|
		^dropMeta.if({ meth[0].name.asString.copyRange(5, 2000) },
				{ meth[1].name })
			++ meth[1].argList(skipThis)
	}

	*itemList { arg mList;
		^mList.collect({ |meth|
			this.listItem(meth);
		});
	}

	*listItem { arg meth;
		^dropMeta.if({ meth[0].name.asString.copyRange(5, 2000) },
				{ meth[0].name })
			++ "-" ++ meth[1].name ++ meth[1].argList(skipThis)
	}

	*restrictList {
		var	str, nametemp, keep;
		str = textField.string;
		reducedList = masterList.select({ |item|
			keep = true;
			nametemp = this.listItem(item);
				// if str is 0 length, loop doesn't execute and all items will be kept
			str.do({ |chr, i|
				(nametemp[i] != chr).if({ keep = false });
			});
			keep
		});
		listView.items_(this.itemList(reducedList)).value_(0);
	}

	*prInit { arg title;
		var boundsTemp, gui;
		\CocoaGUI.asClass.notNil.if({
			gui = CocoaGUI;  // maybe GUI.current will be supportable later
			boundsTemp = gui.window.screenBounds;
				// center the window on screen
			w = gui.window.new(title, Rect(
				(boundsTemp.width - wWidth) / 2, (boundsTemp.height - wHeight) / 2,
				wWidth, wHeight
			)).onClose_({ this.free });
			gui.staticText.new(w, Rect(5, 25, wWidth-10, 20))
				.string_("Type a bit or click and [cr] in the list");
			// 3.2 -> 3.3 transition hack - will hardcode SCTextFieldOld later
			textField = ('SCTextFieldOld'.asClass ?? { SCTextField })
				.new(w, Rect(5, 50, wWidth - 10, 20)).resize_(2);
			listView = gui.listView.new(w, Rect(5, 75, wWidth - 10, wHeight - 80))
				.resize_(5)
				.keyDownAction_({ |listV, char, modifiers, keycode|
					case
						{ (modifiers bitAnd: 10485760 > 0) and: (keycode == 63232) }
							{ listView.value = (listView.value - 1) % reducedList.size }
						{ (modifiers bitAnd: 10485760 > 0) and: (keycode == 63233) }
							{ listView.value = (listView.value + 1) % reducedList.size }					{ char.ascii == 13 } { GUI.use( gui, {this.finish })}
						{ char.ascii == 27 } { this.free }
				});

			textField.keyDownAction_({ |txt, char, modifiers, unicode|
				case
					{ (modifiers bitAnd: 10485760 > 0) and: (unicode == 63232) }
						{ listView.value = (listView.value - 1) % reducedList.size }
					{ (modifiers bitAnd: 10485760 > 0) and: (unicode == 63233) }
						{ listView.value = (listView.value + 1) % reducedList.size }
					{ char.ascii == 13 } { GUI.use( gui, { this.finish })}
					{ char.ascii == 27 } { this.free }
						// default:
					{	txt.defaultKeyDownAction(char, modifiers, unicode);
						this.restrictList(txt.string);
					};
			})
				.action_({ GUI.use( gui, {this.finish })})
				.focus;
			w.front;
		});
	}

}

AutoCompClassBrowser : AutoCompMethodBrowser {
	classvar	savedClass;

	*initClass {
		classExclusions = classExclusions.asArray;
	}

		// an actual array of classes, not symbols: [Nil, Boolean, True, False...]
		// unforgiving of typos!
	*exclude { |classArray|
		classExclusions = classExclusions ++ (classArray.collect({ |cl|
			[cl, ("Meta_" ++ cl.name).asSymbol.asClass]
		}).flat);
	}

	*newCondition { arg start, size, doc;
		var	class;
		class = ("Meta_" ++ doc.string(start, size)).asSymbol;
		^(Document.allowAutoComp and:
			{ classExclusions.includes(class.asClass).not } and:			{ Class.allClasses.detectIndex({ |cl| cl.name == class }).notNil })
	}

	*init { arg argStart, argSize, argDoc;
			// newCondition determines that the class exists, so now it should be
			// ok to interpret the string to get the class
		savedClass = ("Meta_" ++ argDoc.string(argStart, argSize)).interpret;
		reducedList = masterList = this.getMethods(savedClass).sort({ |a, b|
			a.name < b.name
		});
		doc = argDoc;
		start = argStart;
		size = argSize;
		this.prInit(savedClass.name);
		this.restrictList;
		overwriteOnCancel = false;
	}

	*getMethods { arg class;
		var	list, existsFlag;
		list = List.new;
			// loop through superclasses, but only Meta_ classes allowed
		{ class != Class }.while({
				// only if this class is not excluded
			classExclusions.includes(class).not.if({
					// for each method
				class.methods.do({ |meth|
						// does this method name already exist in the list?
						// this accounts for subclasses overriding superclass methods
					existsFlag = false;
					list.do({ |item| (item.name == meth.name).if({ existsFlag = true }); });
						// if not, add to the list
					existsFlag.not.if({ list.add(meth); });
				});
			});
			class = class.superclass;
		});
		^list
	}

	*finishString { |meth|
		var strtemp;
		strtemp = savedClass.name ++ "." ++ meth.name ++ meth.argList;
		^strtemp.copyRange(5, strtemp.size-1);
	}

	*listItem { arg meth;
		^meth.name ++ meth.argList
	}

}

// need to think about this some more
// what if user deletes chars but doesn't add?
// maybe best solution is *free below -- never add chars
// ok because user will not type ctrl-. in normal use

AutoCompClassSearch : AutoCompClassBrowser {
	classvar	classBrowser;	// reserve one class browser for autocomplete use
//			userEditedString = false;

	*newCondition { ^Document.allowAutoComp }	// no restrictions on when a window will be opened

	*init { arg argStart, argSize, argDoc;
		selector = argDoc.string(argStart, argSize);
		reducedList = masterList = Class.allClasses.reject({ |cl|
				cl.isMetaClass or: { classExclusions.includes(cl) } })
			.select({ |cl| cl.name.asString.containsi(selector) })
			.sort({ |a, b| a.name < b.name });
		doc = argDoc;
		start = argStart;
		size = argSize;
		this.prInit("Open a class browser");
		textField.string_(selector);
//		userEditedString = false;
		this.restrictList;
	}

	*free { super.free(true) }

	*finish {
			// I need to test whether the method chosen is in a superclass of the class
			// selected here -- see finishString
		savedClass = reducedList[listView.value];
		classBrowser = savedClass.browse;
			// set enter key action to drop a method template into the document
			// methodView's enter key action can change
			// based on what you do in the browser;
			// save the custom action into the browser instance's environment
			// so that this customization persists
		classBrowser.views.put(\methodEnterKeyAction, {
			var str, newstart, newsize;
				// can't close unless a method is really chosen
			if(classBrowser.currentMethod.notNil) {
				doc.selectRange(start, size)
					.selectedString_(str = this.finishString);
				#newstart, newsize = this.finalSelection(str);
				doc.selectRange(newstart, newsize);
				classBrowser.close;
			};
		});
		classBrowser.views.methodView.focus
			.enterKeyAction_(classBrowser.views[\methodEnterKeyAction])
			.keyDownAction_({ |list, char, modifiers, unicode|
				(char == $^).if({
					classBrowser.views.superButton.doAction;
				}, {
					list.defaultKeyDownAction(char, modifiers, unicode)
				});
			});
		this.free(true);
	}

	*finishString {
		var meth, classname;
		meth = classBrowser.currentMethod;
		if(meth.ownerClass.isMetaClass) {
				// if the method chosen comes from this class or a superclass,
				// use this class, otherwise use the class selected in the class browser
			if(savedClass.metaclass == meth.ownerClass
				or: { savedClass.metaclass.superclasses.includes(meth.ownerClass) }) {
				classname = savedClass.name
			} {
				classname = meth.ownerClass.name.asString.copyRange(5, 2000)
			};
			^classname.asString
				++ "." ++ meth.name ++ meth.argList
		} {
			^meth.name ++ meth.argList
		};
	}

	*listItem { |cl|
		^cl.name.asString
	}
}
