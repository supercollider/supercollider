
AutoCompMethodBrowser {
	classvar	<>wWidth = 400, <>wHeight = 360;
	classvar	methDict;	// note, not the same as Class-methods
					// this is a dictionary: selector -> [method, method, method...]
					// with this, I can look up method selectors almost instantly
					// otherwise, every time I would have to search the whole class tree (slow)
	classvar	methodExclusions, classExclusions;

	classvar	w, textField, listView,		// gui objects
		masterList,				// a list of all methods with that selector
		reducedList,				// only the ones displayed
		skipThis,					// flag: do (this, xxx) or (xxx) in argList
		dropMeta,				// flag: display Meta_ for metaclasses?
		doc,					// document from which this was created
		start, size;	// start and length of identifier in document

	*initClass {
		methDict = IdentityDictionary.new;
		Class.allClasses.do({ |class|
			class.methods.do({ |meth|
					// if this selector hasn't been initialized, make a new set
				methDict[meth.name].isNil.if({ methDict.put(meth.name, Set.new); });
				methDict[meth.name].add(meth);
			});
		});
		methodExclusions = [];
	}
	
		// prevent certain method selector strings from being gui'ed during typing
		// assumes that selectors are spelled correctly!
		// startup.rtf should include AutoCompMethodBrowser.exclude([\value, \if, ...]);
	*exclude { |selectorArray|
		methodExclusions = methodExclusions ++ selectorArray.collect({ |sel| sel.asSymbol });
	}
	
	*new { arg start, size, doc;
			// may only open a browsere if there isn't one open already
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
		var	selector, temp;
		skipThis = dropMeta = false;
		selector = argDoc.string(argStart, argSize);
		(selector.size == 0).if({
			(argDoc.string(argStart-1, 1) == ".").if({
				selector = "value";
			}, { ^nil })	// if no string and prev char was not a ., then abort
		});
			// if it's part of a class name, 
		(selector[0] >= $A and: selector[0] <= $Z).if({
				// identify classes containing that string
			masterList = Class.allClasses.select({ |cl|
				cl.isMetaClass and: { cl.name.asString.containsi(selector) }
			}).collect({ |cl|	// then grab their *new method
				[cl, cl.findRespondingMethodFor(\new)]
			}).reject({ |item| item[1].isNil });
			selector = "new";
			skipThis = dropMeta = true;
		}, {
			masterList = [];
			methDict.keysValuesDo({ |k, v|
				k.asString.containsi(selector).if({
					masterList = masterList ++ v.collect({ |meth| [meth.ownerClass, meth] });
				});
			});
		});
		(masterList.size > 0).if({
				// this sort will sort both of them (desired)
				// [0] is the ownerclass, [1] the method
			reducedList = masterList.sort({ |a, b|
				(a[0].name ++ "-" ++ a[1].name) < (b[0].name ++ "-" ++ b[1].name)
			});
				// if previous char is a ., then class should not be displayed in argList
			(argDoc.string(argStart-1, 1)[0] == $.).if({ skipThis = true });
			doc = argDoc;
			start = argStart;
			size = argSize;
			this.prInit("." ++ selector);
		}, {
			^nil
		});
	}
	
	*free {
			// if window is nil, isclosed should be true
			// close the window only if it isn't closed
		(w.tryPerform(\isClosed) ? true).not.if({ w.close; });
			// garbage; also, w = nil allows next browser to succeed
		w = masterList = reducedList = doc = nil;
	}
	
	*finish {
		var	selectStart, selectSize, str;
			// select the right text in the doc and replace with method template
		doc.selectRange(start, size+1)	// must replace open paren which size doesn't include
			.selectedString_(str = this.finishString(reducedList[listView.value]));
		#selectStart, selectSize = this.finalSelection(str);
		doc.selectRange(selectStart, selectSize);	// reposition cursor
		this.free;
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
		var boundsTemp;
		boundsTemp = SCWindow.screenBounds;
			// center the window on screen
		w = SCWindow.new(title, Rect(
			(boundsTemp.width - wWidth) / 2, (boundsTemp.height - wHeight) / 2,
			wWidth, wHeight
		)).onClose_({ this.free });
		SCStaticText(w, Rect(5, 25, wWidth-10, 20))
			.string_("Type a bit or click and [cr] in the list");
		textField = SCTextField(w, Rect(5, 50, wWidth - 10, 20)).resize_(2);
		listView = SCListView(w, Rect(5, 75, wWidth - 10, wHeight - 80))
			.resize_(5)
			.items_(this.itemList(masterList))
			.keyDownAction_({ |listV, char, modifiers, keycode|
				[{ (modifiers bitAnd: 10485760 > 0) and: (keycode == 63232) }
					-> { listView.value = (listView.value - 1) % reducedList.size },
				 { (modifiers bitAnd: 10485760 > 0) and: (keycode == 63233) }
					-> { listView.value = (listView.value + 1) % reducedList.size },				 { char.ascii == 13 } -> { this.finish },
				 { char.ascii == 27 } -> { this.free }]
				.case;
			});

		textField.keyDownAction_({ |txt, char, modifiers, unicode|
			[{ (modifiers bitAnd: 10485760 > 0) and: (unicode == 63232) }
				-> { listView.value = (listView.value - 1) % reducedList.size },
			 { (modifiers bitAnd: 10485760 > 0) and: (unicode == 63233) }
				-> { listView.value = (listView.value + 1) % reducedList.size },
			 { char.ascii == 13 } -> { this.finish },
			 { char.ascii == 27 } -> { this.free }]
			.case({
				txt.defaultKeyDownAction(char, modifiers, unicode);
				this.restrictList(txt.string);
			});
		})
			.action_({ this.finish })
			.focus;
		w.front;
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

AutoCompClassSearch : AutoCompClassBrowser {
	classvar	classBrowser;	// reserve one class browser for autocomplete use

	*newCondition { ^Document.allowAutoComp }	// no restrictions on when a window will be opened
	
	*init { arg argStart, argSize, argDoc;
		var	selector;
		selector = argDoc.string(argStart, argSize);
		reducedList = masterList = Class.allClasses.reject({ |cl|
				cl.isMetaClass or: { classExclusions.includes(cl) } })
			.select({ |cl| cl.name.asString.containsi(selector) })
			.sort({ |a, b| a.name < b.name });
		doc = argDoc;
		start = argStart;
		size = argSize;
		this.prInit("Open a class browser");
	}
	
	*finish {
			// I need to test whether the method chosen is in a superclass of the class
			// selected here -- see finishString
		savedClass = reducedList[listView.value];
		classBrowser = savedClass.browse;
			// set enter key action to drop a method template into the document
		classBrowser.methodView.focus
			.enterKeyAction_({
				var str, newstart, newsize;
				Document.listener.selectRange(start, size)
					.selectedString_(str = this.finishString);
				#newstart, newsize = this.finalSelection(str);
				Document.listener.selectRange(newstart, newsize);
				classBrowser.methodView.enterKeyAction = classBrowser.normalMethodEnterKey;
				classBrowser.close;
			})
			.keyDownAction_({ |list, char, modifiers, unicode|
				(char == $^).if({
					classBrowser.superButton.doAction;
				}, {
					list.defaultKeyDownAction(char, modifiers, unicode)
				});
			});
		this.free;
	}
	
	*finishString { 
		var meth, classname;
		meth = classBrowser.methodArray[classBrowser.methodView.value];
		meth.ownerClass.isMetaClass.if({
				// if the method chosen comes from this class or a superclass,
				// use this class, otherwise use the class selected in the class browser
			(savedClass.metaclass == meth.ownerClass
				or: { savedClass.metaclass.superclasses.includes(meth.ownerClass) }
			).if({
				classname = savedClass.name
			}, {
				classname = meth.ownerClass.name.asString.copyRange(5, 2000)
			});
				// I'll cheat: rather than get the exact size, this should be faster
			^classname.asString
				++ "." ++ meth.name ++ meth.argList
		}, {
			^meth.name ++ meth.argList
		});
	}
	
	*listItem { |cl|
		^cl.name.asString
	}
}

ClassBrowser {
		// moving jmc's Class-browse method into a separate object
		// with some customizability
		
		// methodView and methodArray need to be accessible from outside
		// (for AutoCompClassSearch)
	var w;
	var currentClassNameView;
	var superClassNameView;
	var subclassView, <methodView, argView;
	var instVarView, classVarView;
	var filenameView;
	var cvsButton;
	var bakButton, fwdButton;
	var <superButton, metaButton, helpButton;
	var methodSourceButton, classSourceButton;
	var implementationButton, refsButton;
	var currentClass;
	var currentMethod;
	var subclassArray, classMethodArray, <methodArray;
	var updateViews, setNewClass;
	var hvBold12;
	var history, historyPos=0;

	*new { arg class;
		^super.new.init(class)
	}

	init { arg class;
		
		currentClass = class;
		history = [currentClass];
		
		w = SCWindow("class browser", Rect(128, 320, 640, 560));
		w.view.decorator = FlowLayout(w.view.bounds);
		
		currentClassNameView = SCTextField(w, Rect(0,0, 256, 32));
		currentClassNameView.font = Font("Helvetica-Bold", 18);
		currentClassNameView.boxColor = Color.clear;
		currentClassNameView.align = \center;
		
		currentClassNameView.action = {
			this.currentClass_( currentClassNameView.string.asSymbol.asClass );
		};
		
		superClassNameView = SCStaticText(w, Rect(0,0, 256, 32));
		superClassNameView.font = hvBold12 = Font("Helvetica-Bold", 12);
		
		w.view.decorator.nextLine;
		
		bakButton = SCButton(w, Rect(0,0, 24, 24));
		bakButton.states = [["<", Color.black, Color.clear]];
		bakButton.action = {
			if (historyPos > 0) {
				historyPos = historyPos - 1;
				currentClass = history[historyPos];
				this.currentClass_ ( currentClass, false );
			}
		};
		
		fwdButton = SCButton(w, Rect(0,0, 24, 24));
		fwdButton.states = [[">", Color.black, Color.clear]];
		fwdButton.action = {
			if (historyPos < (history.size - 1)) {
				historyPos = historyPos + 1;
				currentClass = history[historyPos];
				this.currentClass_ ( currentClass, false );
			}
		};
		
		
		superButton = SCButton(w, Rect(0,0, 50, 24));
		superButton.states = [["super", Color.black, Color.clear]];
		
		superButton.action = {
			this.currentClass_( currentClass.superclass );
		};
		
		metaButton = SCButton(w, Rect(0,0, 50, 24));
		metaButton.states = [["meta", Color.black, Color.clear]];
		
		metaButton.action = {
			this.currentClass_( currentClass.class );
		};
		
		
		helpButton = SCButton(w, Rect(0,0, 50, 24));
		helpButton.states = [["help", Color.black, Color.clear]];
		
		helpButton.action = {	
			if (currentClass.hasHelpFile) {
				currentClass.openHelpFile;
			}
		};
		
		classSourceButton = SCButton(w, Rect(0,0, 90, 24));
		classSourceButton.states = [["class source", Color.black, Color.clear]];
		
		classSourceButton.action = {
			currentClass.openCodeFile;
		};
		
		methodSourceButton = SCButton(w, Rect(0,0, 90, 24));
		methodSourceButton.states = [["method source", Color.black, Color.clear]];
		
		methodSourceButton.action = {
			currentMethod.openCodeFile;
		};
		
		implementationButton = SCButton(w, Rect(0,0, 100, 24));
		implementationButton.states = [["implementations", Color.black, Color.clear]];
		implementationButton.action = {
			thisProcess.interpreter.cmdLine = currentMethod.name.asString;
			thisProcess.methodTemplates;
		};
		
		refsButton = SCButton(w, Rect(0,0, 70, 24));
		refsButton.states = [["references", Color.black, Color.clear]];
		refsButton.action = {
			thisProcess.interpreter.cmdLine = currentMethod.name.asString;
			thisProcess.methodReferences;
		};
		
		cvsButton = SCButton(w, Rect(0,0, 32, 24));
		cvsButton.states = [["cvs", Color.black, Color.clear]];
		cvsButton.action = {
			var filename, cvsAddr;
			cvsAddr = "http://cvs.sourceforge.net/viewcvs.py/"
						"supercollider/SuperCollider3/build/";
			filename = currentClass.filenameSymbol.asString;
			cvsAddr = cvsAddr ++ filename.drop(filename.find("SCClassLibrary"));
			unixCmd("open \"" ++ cvsAddr ++ "\"");
		};
		
		
		w.view.decorator.nextLine;
		
		filenameView = SCStaticText(w, Rect(0,0, 600, 18));
		filenameView.font = Font("Helvetica", 10);
		
		
		w.view.decorator.nextLine;
		SCStaticText(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("class vars");
		SCStaticText(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("instance vars");
		w.view.decorator.nextLine;
		
		classVarView = SCListView(w, Rect(0,0, 180, 130));
		instVarView = SCListView(w, Rect(0,0, 180, 130));
		classVarView.value = 0;
		instVarView.value = 0;
		w.view.decorator.nextLine;
		
		SCStaticText(w, Rect(0,0, 220, 24))
			.font_(hvBold12).align_(\center).string_("subclasses  (press return)");
		SCStaticText(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("methods");
		SCStaticText(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("arguments");
		w.view.decorator.nextLine;
		
		subclassView = SCListView(w, Rect(0,0, 220, 260));
		methodView = SCListView(w, Rect(0,0, 180, 260));
		argView = SCListView(w, Rect(0,0, 180, 260));
		subclassView.resize = 4;
		methodView.resize = 4;
		argView.resize = 4;
		
		w.view.decorator.nextLine;
		
		subclassView.enterKeyAction = {
			this.currentClass_( subclassArray[subclassView.value] );
		};
		methodView.enterKeyAction = this.normalMethodEnterKey;
		
		methodView.action = {
			var nextMethod;
			nextMethod = methodArray[methodView.value];
			if (nextMethod.notNil) {
				currentMethod = nextMethod;
				this.updateViews;
			}
		};
		
		this.updateViews;
		
		w.front;
	}
	
	close {
		w.close
	}
	
	normalMethodEnterKey {
		^{ currentMethod.openCodeFile; }
	}

	currentClass_ { | newClass, addHistory = true |
		if (newClass.notNil) {
			if (addHistory) {
				if (history.size > 1000) { history = history.drop(1) };
				history = history.extend(historyPos + 1).add(newClass);
				historyPos = historyPos + 1;
			};
			currentClass = newClass;
			subclassView.value = 0;
			methodView.value = 0;
			argView.value = 0;
			this.updateViews;
		};
	}
	
	updateViews {
		var classMethodNames, methodNames;
		
		currentClassNameView.string = currentClass.name.asString;
	
		if (currentClass.superclass.notNil) {
			superClassNameView.string = "superclass: "
					++ currentClass.superclass.name.asString;
		}{
			superClassNameView.string = "superclass: nil";
		};
	
		if (currentClass.subclasses.isNil) { subclassArray = []; }{
			subclassArray = currentClass.subclasses.copy.sort {|a,b| a.name <= b.name };
		};
		subclassView.items = subclassArray.collect({|class| class.name.asString });
		
		if (currentClass.class.methods.isNil) { classMethodArray = []; }{
			classMethodArray = currentClass.class.methods.asArray.copy.sort
								{|a,b| a.name <= b.name };
								
			classMethodNames = classMethodArray.collect {|method|
				"*" ++ method.name.asString
			};
		};
		
		if (currentClass.methods.isNil) { methodArray = [] }{
			methodArray = currentClass.methods.asArray.copy.sort
								{|a,b| a.name <= b.name };
								
			methodNames = methodArray.collect {|method|
				method.name.asString
			};
		};
		
		methodArray = classMethodArray ++ methodArray;
		methodView.items = classMethodNames ++ methodNames;
		
		currentMethod = methodArray[methodView.value];
		
		if (currentMethod.isNil or: { currentMethod.argNames.isNil }) {
			argView.items = ["this"];
		}{
			argView.items = currentMethod.argNames.collectAs( {|name, i|
				var defval;
				defval = currentMethod.prototypeFrame[i];
				if (defval.isNil) { name.asString }{
					name.asString ++ "     = " ++ defval.asString
				}
			}, Array);
		};
		
		classVarView.items =
			currentClass.classVarNames.asArray.collectAs
				({|name| name.asString }, Array).sort;
	
		instVarView.items =
			currentClass.instVarNames.asArray.collectAs
				({|name| name.asString }, Array).sort;
				
		filenameView.string = currentClass.filenameSymbol.asString;
	}

}
