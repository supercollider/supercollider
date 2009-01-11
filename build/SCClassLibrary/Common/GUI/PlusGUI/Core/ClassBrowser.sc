
ClassBrowser {
		// moving jmc's Class-browse method into a separate object
		// with some customizability
		
		// methodView and methodArray need to be accessible from outside
		// (for AutoCompClassSearch)
	var w;
	var currentClassNameView;
	var superClassNameView;
	var subclassTitle, subclassView, <methodView, argView;
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
	var searchViews, searchField, searchMenu, searchButton, matchCaseButton;

	*new { arg class;
		^super.new.init(class)
	}

	init { arg class;
		var gui;
		
		currentClass = class;
		history = [currentClass];
		
		gui	= GUI.current;
		
		w = gui.window.new("class browser", Rect(128, 320, 640, 560))
			.onClose_({ this.free });

		w.view.decorator = FlowLayout(w.view.bounds);
		
		currentClassNameView = gui.textField.new(w, Rect(0,0, 256, 32));
		currentClassNameView.font = gui.font.new( gui.font.defaultSansFace, 18 ).boldVariant;
		currentClassNameView.background = Color.clear;
		currentClassNameView.align = \center;
		
		currentClassNameView.action = {
			this.currentClass_( currentClassNameView.string.asSymbol.asClass );
		};
		
		superClassNameView = gui.staticText.new(w, Rect(0,0, 256, 32));
		superClassNameView.font = hvBold12 = gui.font.new( gui.font.defaultSansFace, 12 ).boldVariant;
		
		w.view.decorator.nextLine;
		
		bakButton = gui.button.new(w, Rect(0,0, 24, 24));
		bakButton.states = [["<", Color.black, Color.clear]];
		bakButton.action = {
			if (historyPos > 0) {
				historyPos = historyPos - 1;
				this.restoreHistory;
			}
		};
		
		fwdButton = gui.button.new(w, Rect(0,0, 24, 24));
		fwdButton.states = [[">", Color.black, Color.clear]];
		fwdButton.action = {
			if (historyPos < (history.size - 1)) {
				historyPos = historyPos + 1;
				this.restoreHistory;
			}
		};
		
		
		superButton = gui.button.new(w, Rect(0,0, 50, 24));
		superButton.states = [["super", Color.black, Color.clear]];
		
		superButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass_( this.currentClass.superclass );
			}
		};
		
		metaButton = gui.button.new(w, Rect(0,0, 50, 24));
		metaButton.states = [["meta", Color.black, Color.clear]];
		
		metaButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass_( this.currentClass.class );
			};
		};
		
		
		helpButton = gui.button.new(w, Rect(0,0, 50, 24));
		helpButton.states = [["help", Color.black, Color.clear]];
		
		helpButton.action = {	
			if(this.currentClass.notNil) {
				this.currentClass.openHelpFile;
			}
		};
		
		classSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		classSourceButton.states = [["class source", Color.black, Color.clear]];
		
		classSourceButton.action = {
			if(this.currentClass.notNil) {
				this.currentClass.openCodeFile;
			}
		};
		
		methodSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		methodSourceButton.states = [["method source", Color.black, Color.clear]];
		
		methodSourceButton.action = {
			if(currentClass.isArray.not) {
				currentMethod.openCodeFile;
			};
		};
		
		implementationButton = gui.button.new(w, Rect(0,0, 100, 24));
		implementationButton.states = [["implementations", Color.black, Color.clear]];
		implementationButton.action = {
			if(currentClass.isArray.not) {
				thisProcess.interpreter.cmdLine = currentMethod.name.asString;
				thisProcess.methodTemplates;
			};
		};
		
		refsButton = gui.button.new(w, Rect(0,0, 70, 24));
		refsButton.states = [["references", Color.black, Color.clear]];
		refsButton.action = {
			if(currentClass.isArray.not) {
				thisProcess.interpreter.cmdLine = currentMethod.name.asString;
				thisProcess.methodReferences;
			};
		};
		
		if(this.respondsTo(\openSVN)) {
			svnButton = gui.button.new(w, Rect(0,0, 32, 24));
			svnButton.states = [["svn", Color.black, Color.clear]];
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

		searchViews = GUI.compositeView.new(w, Rect(0, 0, 205, 80));
		searchViews.decorator = FlowLayout(searchViews.bounds, Point(1, 1));
		searchButton = GUI.button.new(searchViews, Rect(0, 0, 60, 20))
			.states_([["Search", Color.black, Color.clear]])
			.action_({
				this.searchClasses(searchField.string, searchMenu.value, matchCaseButton.value);
			});
		matchCaseButton = GUI.button.new(searchViews, Rect(0, 0, 100, 20))
			.states_([["Case insensitive", Color.black, Color.clear], ["Match case", Color.black, Color.clear]]);
		searchViews.decorator.nextLine;
		searchField = GUI.textField.new(searchViews, Rect(0, 0, 200, 20))
			.string_("Search for...")
			.action_({
				this.searchClasses(searchField.string, searchMenu.value, matchCaseButton.value);
			});
		searchMenu = GUI.popUpMenu.new(searchViews, Rect(0, 0, 200, 20))
			.items_(this.makeSearchItems(this.currentClass));

		w.view.decorator.nextLine;
		
		subclassTitle = gui.staticText.new(w, Rect(0,0, 220, 24))
			.font_(hvBold12).align_(\center).string_("subclasses  (press return)");
		gui.staticText.new(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("methods");
		gui.staticText.new(w, Rect(0,0, 180, 24))
			.font_(hvBold12).align_(\center).string_("arguments");
		w.view.decorator.nextLine;
		
		subclassView = gui.listView.new(w, Rect(0,0, 220, 260));
		methodView = gui.listView.new(w, Rect(0,0, 180, 260));
		argView = gui.listView.new(w, Rect(0,0, 180, 260));
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

//	override in platform specific extension
//
//	openSVN{ |svnAddr|
//		// rely on extension in platform dependent extension
//		"WARNING: not implemented for this platform".postln;
//		("open the url:"+svnAddr+"in your browser").postln;
//	}

	currentClass {
		^if(currentClass.isArray) {
			currentClass[subclassView.value]
		} { currentClass };
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

	restoreHistory {
		this.currentClass_(history[historyPos], false);
	}
	
	updateViews {
		var classMethodNames, methodNames;
		
		if(currentClass.isArray) {
			this.updateViewsForSearch(currentClass);
		} {
			currentClassNameView.string = currentClass.name.asString;
			subclassTitle.string = "subclasses  (press return)";
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
			subclassView.action = nil;

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

			searchMenu.items = this.makeSearchItems(currentClass);

			methodSourceButton.states = [["method source", Color.black, Color.clear]];
			implementationButton.states = [["implementations", Color.black, Color.clear]];
			refsButton.states = [["references", Color.black, Color.clear]];
			superButton.states = [["super", Color.black, Color.clear]];
			metaButton.states = [["meta", Color.black, Color.clear]];
			helpButton.states = [["help", Color.black, Color.clear]];
			classSourceButton.states = [["class source", Color.black, Color.clear]];
			svnButton !? { svnButton.states = [["svn", Color.black, Color.clear]] };
			w.refresh;
		}
	}

	updateViewsForSearch { |searchResults|
		var classMethodNames, methodNames;
		
		currentClassNameView.string = "Search results";
		superClassNameView.string = "";

		subclassArray = searchResults;
		subclassView.items_(searchResults.collect({|class| class.name.asString }))
			.value_(0)
			.action_({
				searchMenu.items = this.makeSearchItems(this.currentClass);
			});

		classMethodArray = [];
		methodArray = [];
		methodView.items = [];
		
		currentMethod = nil;
		argView.items = [];
		classVarView.items = [];
		instVarView.items = [];
				
		filenameView.string = "";
		searchMenu.items_(this.makeSearchItems(searchResults[0]));

		methodSourceButton.states = [["method source", Color.gray, Color.clear]];
		implementationButton.states = [["implementations", Color.gray, Color.clear]];
		refsButton.states = [["references", Color.gray, Color.clear]];
		if(searchResults.isEmpty) {
			subclassTitle.string = "no matching classes (try again)";
			superButton.states = [["super", Color.gray, Color.clear]];
			metaButton.states = [["meta", Color.gray, Color.clear]];
			helpButton.states = [["help", Color.gray, Color.clear]];
			classSourceButton.states = [["class source", Color.gray, Color.clear]];
			svnButton !? { svnButton.states = [["svn", Color.gray, Color.clear]] };
			searchMenu.value = 0;
		} {
			subclassTitle.string = "matching classes (press return)";
			superButton.states = [["super", Color.black, Color.clear]];
			metaButton.states = [["meta", Color.black, Color.clear]];
			helpButton.states = [["help", Color.black, Color.clear]];
			classSourceButton.states = [["class source", Color.black, Color.clear]];
			svnButton !? { svnButton.states = [["svn", Color.black, Color.clear]] };
		};
		w.refresh;
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
		^if(class.notNil) {
			[	"All classes",
				"Subclasses of " ++ class.name,
				"Folder " ++ PathName(class.filenameSymbol.asString).allFolders.last
			]
		} {
			#["All classes"]
		};
	}

	searchClasses { |string, rootNumber, matchCase, addHistory = true|
		var	pool, rootdir;
		matchCase = matchCase > 0;
		pool = switch(rootNumber)
			{ 0 } {
				Class.allClasses
			}
			{ 1 } {
				this.currentClass.allSubclasses
			}
			{ 2 } {
				rootdir = PathName(this.currentClass.filenameSymbol.asString).pathOnly;
				Class.allClasses.select({ |class|
					PathName(class.filenameSymbol.asString).pathOnly == rootdir
				});
			};
		this.currentClass = (if(matchCase) {
				pool.select({ |class|
					class.isMetaClass.not and: { class.name.asString.contains(string) }
				})
			} {
				pool.select({ |class|
					class.isMetaClass.not and: { class.name.asString.containsi(string) }
				})
			}).sort({ |a, b| a.name < b.name });
	}
}
