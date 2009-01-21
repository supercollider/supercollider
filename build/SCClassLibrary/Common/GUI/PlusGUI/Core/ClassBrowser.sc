
ClassBrowser {
		// avoid creating extra garbage by constantly creating new Color objects
	classvar	black, gray, clear;
		// moving jmc's Class-browse method into a separate object
		// with some customizability
		
		// methodView and methodArray need to be accessible from outside
		// (for AutoCompClassSearch)
	var w;
	var currentClassNameView;
	var superClassNameView;
	var subclassTitle, subclassView, methodTitle, <methodView, argView;
	var instVarView, classVarView;
	var filenameView;
	var svnButton;
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
	var searchViews, searchField, searchMenu, searchButton, matchCaseButton, isClassSearch;

	*initClass {
		black = Color.black;
		gray = Color.gray;
		clear = Color.clear;
	}

	*new { arg class;
		^super.new.init(class)
	}

	init { arg class;
		var gui;
		
		currentClass = class;
		history = [currentClass];
		
		gui	= GUI.current;
		
		w = gui.window.new("class browser", Rect(128, (GUI.window.screenBounds.height - 638).clip(0, 320), 720, 600))
			.onClose_({ this.free });

		w.view.decorator = FlowLayout(w.view.bounds);
		
		currentClassNameView = gui.textField.new(w, Rect(0,0, 256, 32));
		currentClassNameView.font = gui.font.new( gui.font.defaultSansFace, 18 ).boldVariant;
		currentClassNameView.background = clear;
		currentClassNameView.align = \center;
		
		currentClassNameView.action = {
			this.currentClass_( currentClassNameView.string.asSymbol.asClass );
		};
		
		superClassNameView = gui.staticText.new(w, Rect(0,0, 256, 32));
		superClassNameView.font = hvBold12 = gui.font.new( gui.font.defaultSansFace, 12 ).boldVariant;
		
		w.view.decorator.nextLine;

		bakButton = gui.button.new(w, Rect(0,0, 24, 24));
		bakButton.states = [["<", black, clear]];
		bakButton.action = {
			if (historyPos > 0) {
				historyPos = historyPos - 1;
				this.restoreHistory;
			}
		};
		
		fwdButton = gui.button.new(w, Rect(0,0, 24, 24));
		fwdButton.states = [[">", black, clear]];
		fwdButton.action = {
			if (historyPos < (history.size - 1)) {
				historyPos = historyPos + 1;
				this.restoreHistory;
			}
		};
		
		
		superButton = gui.button.new(w, Rect(0,0, 50, 24));
		superButton.states = [["super", black, clear]];
		
		superButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass_( this.currentClass.superclass );
			}
		};
		
		metaButton = gui.button.new(w, Rect(0,0, 50, 24));
		metaButton.states = [["meta", black, clear]];
		
		metaButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass_( this.currentClass.class );
			};
		};
		
		
		helpButton = gui.button.new(w, Rect(0,0, 50, 24));
		helpButton.states = [["help", black, clear]];
		
		helpButton.action = {	
			if(this.currentClass.notNil) {
				this.currentClass.openHelpFile;
			}
		};
		
		classSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		classSourceButton.states = [["class source", black, clear]];
		
		classSourceButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass.openCodeFile;
			}
		};
		
		methodSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		methodSourceButton.states = [["method source", black, clear]];
		
		methodSourceButton.action = {
			if(isClassSearch.not or: { currentClass.isArray.not }) {
				currentMethod.openCodeFile;
			};
		};
		
		implementationButton = gui.button.new(w, Rect(0,0, 100, 24));
		implementationButton.states = [["implementations", black, clear]];
		implementationButton.action = {
			if(isClassSearch.not or: { currentClass.isArray.not }) {
				thisProcess.interpreter.cmdLine = currentMethod.name.asString;
				thisProcess.methodTemplates;
			};
		};
		
		refsButton = gui.button.new(w, Rect(0,0, 70, 24));
		refsButton.states = [["references", black, clear]];
		refsButton.action = {
			if(isClassSearch.not or: { currentClass.isArray.not }) {
				thisProcess.interpreter.cmdLine = currentMethod.name.asString;
				thisProcess.methodReferences;
			};
		};
		
		if(this.respondsTo(\openSVN)) {
			svnButton = gui.button.new(w, Rect(0,0, 32, 24));
			svnButton.states = [["svn", black, clear]];
			svnButton.action = {
				var filename, svnAddr;
				if(this.currentClass.notNil) {
					svnAddr = "http://supercollider.svn.sourceforge.net/viewvc/supercollider/trunk/build/";
					filename = this.currentClass.filenameSymbol.asString;
					svnAddr = svnAddr ++ filename.drop(filename.find("SCClassLibrary"));
					svnAddr = svnAddr ++ "?view=log";
					this.openSVN( svnAddr );
				};
			};
		};		
		
		w.view.decorator.nextLine;
		
//		searchViews = GUI.compositeView.new(w, Rect(0, 0, 205, 80));
//		searchViews.decorator = FlowLayout(searchViews.bounds, Point(1, 1));

// for testing
searchViews = w;
		GUI.staticText.new(w, Rect(0, 0, 65, 20)).string_("Search for");
		searchField = GUI.textField.new(searchViews, Rect(0, 0, 235, 20))
//			.string_("Search for...")
			.action_({
				this.searchClasses(searchField.string, searchMenu.value, matchCaseButton.value);
			});
		GUI.staticText.new(w, Rect(0, 0, 15, 20)).string_("in").align_(\center);
		searchMenu = GUI.popUpMenu.new(searchViews, Rect(0, 0, 200, 20))
			.items_(this.makeSearchItems(this.currentClass));
//		GUI.staticText.new(w, Rect(0, 0, 100, 20)).string_("Search for");
		matchCaseButton = GUI.button.new(searchViews, Rect(0, 0, 100, 20))
			.states_([["Case insensitive", black, clear], ["Match case", black, clear]]);

		searchButton = GUI.button.new(searchViews, Rect(0, 0, 40, 20))
			.states_([["GO", black, clear]])
			.action_({
				this.searchClasses(searchField.string, searchMenu.value, matchCaseButton.value);
			});
//		searchViews.decorator.nextLine;

		w.view.decorator.nextLine;

		filenameView = gui.staticText.new(w, Rect(0,0, 600, 18));
		filenameView.font = gui.font.new( gui.font.defaultSansFace, 10 );
		
		w.view.decorator.nextLine;
		gui.staticText.new(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("class vars");
		gui.staticText.new(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("instance vars");
		w.view.decorator.nextLine;
		
		classVarView = gui.listView.new(w, Rect(0,0, 180, 130));
		instVarView = gui.listView.new(w, Rect(0,0, 180, 130));
		classVarView.value = 0;
		instVarView.value = 0;

		w.view.decorator.nextLine;
		
		subclassTitle = gui.staticText.new(w, Rect(0,0, 220, 24))
			.font_(hvBold12).align_(\center).string_("subclasses  (press return)");
		methodTitle = gui.staticText.new(w, Rect(0,0, 240, 24))
			.font_(hvBold12).align_(\center).string_("methods");
		gui.staticText.new(w, Rect(0,0, 200, 24))
			.font_(hvBold12).align_(\center).string_("arguments");
		w.view.decorator.nextLine;
		
		subclassView = gui.listView.new(w, Rect(0,0, 220, 260));
		methodView = gui.listView.new(w, Rect(0,0, 240, 260));
		argView = gui.listView.new(w, Rect(0,0, 200, 260));
		subclassView.resize = 4;
		methodView.resize = 4;
		argView.resize = 4;
		
		w.view.decorator.nextLine;
		
		subclassView.enterKeyAction = {
			this.currentClass_( subclassArray[subclassView.value] );
		};
		subclassView.mouseDownAction_{|v,x, y, modifiers, buttonNumber, clickCount|			if(clickCount==2){
				v.enterKeyAction.value;
			}
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

//	override in platform specific extension
//
//	openSVN{ |svnAddr|
//		// rely on extension in platform dependent extension
//		"WARNING: not implemented for this platform".postln;
//		("open the url:"+svnAddr+"in your browser").postln;
//	}

	currentClass {
		^if(currentClass.isArray) {
			if(currentClass.first.isNil or: { currentClass.first.isKindOf(Class) }) {
				currentClass[subclassView.value]
			} {
				currentClass[methodView.value].ownerClass;
			}
		} { currentClass };
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

	restoreHistory {
		isClassSearch = history[historyPos].isArray
			and: { history[historyPos].first.isKindOf(Class) };
		this.currentClass_(history[historyPos], false);
	}
	
		// normally class arg is nil and uses currentClass
		// but class search subclassView can update methodView
		// so 'class' can override currentClass
	updateViews { |class|
		var classMethodNames, methodNames, classWasNil = true;

		if(class.isNil and: { currentClass.isArray }) {
			this.updateViewsForSearch(currentClass);
		} {
			if(class != \emptySearch) {
				if(class.isNil) { class = currentClass } { classWasNil = false };
			
				currentClassNameView.string = class.name.asString;
				subclassTitle.string = "subclasses (press return)";
				if (class.superclass.notNil) {
					superClassNameView.string = "superclass: "
							++ class.superclass.name.asString;
				}{
					superClassNameView.string = "superclass: nil";
				};
			
				if(classWasNil) {
					if (class.subclasses.isNil) { subclassArray = []; }{
						subclassArray = class.subclasses.copy.sort {|a,b| a.name <= b.name };
					};
					subclassView.items = subclassArray.collect({|class| class.name.asString });
					subclassView.action = nil;
				};
	
				if(currentClass.isArray.not or: { currentClass.first.isKindOf(Class) }) {
					if (class.class.methods.isNil) { classMethodArray = []; }{
						classMethodArray = class.class.methods.asArray.copy.sort
											{|a,b| a.name <= b.name };
											
						classMethodNames = classMethodArray.collect {|method|
							"*" ++ method.name.asString
						};
					};
					
					if (class.methods.isNil) { methodArray = [] }{
						methodArray = class.methods.asArray.copy.sort
											{|a,b| a.name <= b.name };
											
						methodNames = methodArray.collect {|method|
							method.name.asString
						};
					};
				
					methodArray = classMethodArray ++ methodArray;
					methodView.items = classMethodNames ++ methodNames;
				};
				
				currentMethod = methodArray[methodView.value];
				this.updateArgView(currentMethod);
				
				classVarView.items =
					class.classVarNames.asArray.collectAs
						({|name| name.asString }, Array).sort;
			
				instVarView.items =
					class.instVarNames.asArray.collectAs
						({|name| name.asString }, Array).sort;
						
				filenameView.string = class.filenameSymbol.asString;
	
				searchMenu.items = this.makeSearchItems(class);
	
				if(classWasNil) {
					methodTitle.string = "methods";
					methodSourceButton.states = [["method source", black, clear]];
					implementationButton.states = [["implementations", black, clear]];
					refsButton.states = [["references", black, clear]];
					superButton.states = [["super", black, clear]];
					metaButton.states = [["meta", black, clear]];
					helpButton.states = [["help", black, clear]];
					classSourceButton.states = [["class source", black, clear]];
					svnButton !? { svnButton.states = [["svn", black, clear]] };
				};
				w.refresh;
			} {
					// search was empty, clear everything
				subclassArray = [];
				subclassView.items_([]).value_(0);
				methodArray = [];
				methodView.items_([]).value_(0);
				classVarView.items = [];
				instVarView.items = [];
				argView.items = [];
				filenameView.string = "";
				searchMenu.items = this.makeSearchItems(nil);
				methodSourceButton.states = [["method source", gray, clear]];
				implementationButton.states = [["implementations", gray, clear]];
				refsButton.states = [["references", gray, clear]];
				superButton.states = [["super", gray, clear]];
				metaButton.states = [["meta", gray, clear]];
				helpButton.states = [["help", gray, clear]];
				classSourceButton.states = [["class source", gray, clear]];
				svnButton !? { svnButton.states = [["svn", gray, clear]] };
			}
		}
	}
	
	updateArgView { |method|
		if (method.isNil or: { method.argNames.isNil }) {
			argView.items = ["this"];
		}{
			argView.items = method.argNames.collectAs( {|name, i|
				var defval;
				defval = method.prototypeFrame[i];
				if (defval.isNil) { name.asString }{
					name.asString ++ "     = " ++ defval.asString
				}
			}, Array);
		};
	}

	updateViewsForSearch { |searchResults|
		var classMethodNames, methodNames, methodRelatedColor;
		
		currentClassNameView.string = "Search results";
		superClassNameView.string = "";

		searchMenu.items_(this.makeSearchItems(searchResults[0]));

		if(isClassSearch) {
			subclassArray = searchResults;
			subclassView.items_(searchResults.collect({|class| class.name.asString }))
				.value_(0)
				.action_({ |view|
					var	temp;
					searchMenu.items = this.makeSearchItems(this.currentClass);
					temp = subclassArray[view.value].tryPerform(\filenameSymbol);
					filenameView.string = if(temp.isNil) { "" } { temp.asString };
					this.updateViews(subclassArray[view.value] ? \emptySearch);
				});

			subclassView.doAction;
			methodTitle.string = "methods";
//			classMethodArray = [];
//			methodArray = [];
//			methodView.items = [];
//			
//			currentMethod = nil;
//			argView.items = [];
//			classVarView.items = [];
//			instVarView.items = [];
			searchMenu.value = 0;
		} {
			methodArray = searchResults;
			methodTitle.string = "methods (return->class)";
			methodView
				.items_(methodArray.collect({ |method|
					method.name ++ " (" ++ method.ownerClass.name ++ ")"
				}))
				.value_(0)
				.action_({ |view|
					var	method = methodArray[view.value], class, temp;
					if(method.notNil) {
						currentMethod = method;
						class = this.getClass(currentMethod);
//						this.updateArgView(currentMethod);
						temp = currentMethod.tryPerform(\filenameSymbol);
						filenameView.string = if(temp.isNil) { "" } { temp.asString };
						subclassView.value_(subclassArray.indexOf(class) ? 0);
						this.updateViews(class ? \emptySearch);
					} {
						this.updateViews(\emptySearch);
					};
				})
				.enterKeyAction_({ |view|
					var	method = methodArray[view.value];
					if(method.notNil) {
						this.currentClass = this.getClass(method);
						view.enterKeyAction = this.normalMethodEnterKey;
						searchMenu.value = 0;
					};
				});
			methodView.doAction;	// this will also set currentMethod
			
			subclassArray = IdentitySet.new
				.addAll(methodArray.collect { |method| this.getClass(method) })
				.asArray.sort({ |a, b| a.name < b.name });
			subclassView.items_(subclassArray.collect(_.asString))
				.value_(subclassArray.indexOf(this.getClass(currentMethod)) ? 0)
				.action_(nil);
			
			searchMenu.value = 1;
		};

		methodRelatedColor = if(isClassSearch) { gray } { black };
		methodSourceButton.states = [["method source", methodRelatedColor, clear]];
		implementationButton.states = [["implementations", methodRelatedColor, clear]];
		refsButton.states = [["references", methodRelatedColor, clear]];
		if(searchResults.isEmpty) {
			subclassTitle.string = "no matches found (try again)";
			superButton.states = [["super", gray, clear]];
			metaButton.states = [["meta", gray, clear]];
			helpButton.states = [["help", gray, clear]];
			classSourceButton.states = [["class source", gray, clear]];
			svnButton !? { svnButton.states = [["svn", gray, clear]] };
			searchMenu.value = 0;
		} {
			subclassTitle.string = "matching classes (press return)";
			superButton.states = [["super", black, clear]];
			metaButton.states = [["meta", black, clear]];
			helpButton.states = [["help", black, clear]];
			classSourceButton.states = [["class source", black, clear]];
			svnButton !? { svnButton.states = [["svn", black, clear]] };
		};
		w.refresh;
	}
	
		// workaround for the fact that you can't get a non-metaclass from a metaclass
	getClass { |method|
		var	class;
		if(method.isNil) { ^nil };
		^if((class = method.ownerClass).isMetaClass) {
			class.name.asString[5..].asSymbol.asClass;
		} { class }
	}
	
	free {
		(w.notNil and: { w.isClosed.not }).if({ w.close });
		w = currentClassNameView = superClassNameView = subclassView = methodView = argView
			= instVarView = classVarView = filenameView = svnButton = bakButton = fwdButton
			= superButton = metaButton = helpButton = methodSourceButton = classSourceButton
			= implementationButton = refsButton = currentClass = currentMethod = subclassArray
			= classMethodArray = methodArray = updateViews = setNewClass = hvBold12
			= history = historyPos = nil;
	}

	//////// Support methods for class searches

	makeSearchItems { |class|
		^if(class.isKindOf(Class)) {
			[	"All classes",
				"All methods",
				"Subclasses of " ++ class.name,
				"% + subclass methods".format(class.name),
				"Classes in folder " ++ PathName(class.filenameSymbol.asString).allFolders.last
			]
		} {
			#["All classes", "All methods"]
		};
	}

	searchClasses { |string, rootNumber, matchCase, addHistory = true|
		var	pool, rootdir, result;
		string = string ?? { "" };
		matchCase = matchCase > 0;
			// hack, ugh
//		if(searchMenu.items.size == 2) { rootNumber = rootNumber * 3 };
		switch(rootNumber)
			{ 0 } {
				isClassSearch = true;
				pool = Class.allClasses
			}
			{ 1 } {
				isClassSearch = false;
				pool = Class.allClasses;
			}
			{ 2 } {
				isClassSearch = true;
				pool = this.currentClass.allSubclasses
			}
			{ 3 } {
				isClassSearch = false;
				pool = this.currentClass.allSubclasses ++ this.currentClass
			}
			{ 4 } {
				isClassSearch = true;
				rootdir = PathName(this.currentClass.filenameSymbol.asString).pathOnly;
				pool = Class.allClasses.select({ |class|
					PathName(class.filenameSymbol.asString).pathOnly == rootdir
				});

				if(string.isEmpty) {
					this.currentClass = pool.reject(_.isMetaClass).sort({ |a, b| a.name < b.name });
					^this
				};
			};
		if(isClassSearch) {
			this.currentClass = (if(matchCase) {
				pool.select({ |class|
					class.isMetaClass.not and: { class.name.asString.contains(string) }
				})
			} {
				pool.select({ |class|
					class.isMetaClass.not and: { class.name.asString.containsi(string) }
				})
			}).sort({ |a, b| a.name < b.name });
		} {
			result = Array.new;
			if(matchCase) {
				pool.do({ |class|
					result = result.addAll(class.methods.select({ |method|
						method.name.asString.contains(string)
					}));
				});
			} {
				pool.do({ |class|
					result = result.addAll(class.methods.select({ |method|
						method.name.asString.containsi(string)
					}));
				});
			};
			this.currentClass = result.sort({ |a, b|
				if(a.name == b.name) { a.ownerClass.name < b.ownerClass.name }
					{ a.name < b.name };
			});
		}
	}

	normalMethodEnterKey {
		^{ currentMethod.openCodeFile; }
	}
}
