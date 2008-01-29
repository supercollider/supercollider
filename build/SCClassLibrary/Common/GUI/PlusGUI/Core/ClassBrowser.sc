
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
		currentClassNameView.boxColor = Color.clear;
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
				currentClass = history[historyPos];
				this.currentClass_ ( currentClass, false );
			}
		};
		
		fwdButton = gui.button.new(w, Rect(0,0, 24, 24));
		fwdButton.states = [[">", Color.black, Color.clear]];
		fwdButton.action = {
			if (historyPos < (history.size - 1)) {
				historyPos = historyPos + 1;
				currentClass = history[historyPos];
				this.currentClass_ ( currentClass, false );
			}
		};
		
		
		superButton = gui.button.new(w, Rect(0,0, 50, 24));
		superButton.states = [["super", Color.black, Color.clear]];
		
		superButton.action = {
			this.currentClass_( currentClass.superclass );
		};
		
		metaButton = gui.button.new(w, Rect(0,0, 50, 24));
		metaButton.states = [["meta", Color.black, Color.clear]];
		
		metaButton.action = {
			this.currentClass_( currentClass.class );
		};
		
		
		helpButton = gui.button.new(w, Rect(0,0, 50, 24));
		helpButton.states = [["help", Color.black, Color.clear]];
		
		helpButton.action = {	
			if (currentClass.hasHelpFile) {
				currentClass.openHelpFile;
			}
		};
		
		classSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		classSourceButton.states = [["class source", Color.black, Color.clear]];
		
		classSourceButton.action = {
			currentClass.openCodeFile;
		};
		
		methodSourceButton = gui.button.new(w, Rect(0,0, 90, 24));
		methodSourceButton.states = [["method source", Color.black, Color.clear]];
		
		methodSourceButton.action = {
			currentMethod.openCodeFile;
		};
		
		implementationButton = gui.button.new(w, Rect(0,0, 100, 24));
		implementationButton.states = [["implementations", Color.black, Color.clear]];
		implementationButton.action = {
			thisProcess.interpreter.cmdLine = currentMethod.name.asString;
			thisProcess.methodTemplates;
		};
		
		refsButton = gui.button.new(w, Rect(0,0, 70, 24));
		refsButton.states = [["references", Color.black, Color.clear]];
		refsButton.action = {
			thisProcess.interpreter.cmdLine = currentMethod.name.asString;
			thisProcess.methodReferences;
		};
		
		svnButton = gui.button.new(w, Rect(0,0, 32, 24));
		svnButton.states = [["svn", Color.black, Color.clear]];
		svnButton.action = {
			var filename, svnAddr;
			svnAddr = "http://supercollider.svn.sourceforge.net/viewvc/supercollider/trunk/build/";
			filename = currentClass.filenameSymbol.asString;
			svnAddr = svnAddr ++ filename.drop(filename.find("SCClassLibrary"));
			this.openSVN( svnAddr );
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
		w.view.decorator.nextLine;
		
		gui.staticText.new(w, Rect(0,0, 220, 24))
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
	
	free {
		(w.notNil and: { w.isClosed.not }).if({ w.close });
		w = currentClassNameView = superClassNameView = subclassView = methodView = argView
			= instVarView = classVarView = filenameView = svnButton = bakButton = fwdButton
			= superButton = metaButton = helpButton = methodSourceButton = classSourceButton
			= implementationButton = refsButton = currentClass = currentMethod = subclassArray
			= classMethodArray = methodArray = updateViews = setNewClass = hvBold12 = history 			= historyPos = nil;
	}

}
