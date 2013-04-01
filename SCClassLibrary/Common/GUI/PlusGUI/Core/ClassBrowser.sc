// A thorough refactoring of JMc's original class browser
// Previously a state was a Class or an array representing a search result
// Now it's an Environment holding more info
// James Harkins

ClassBrowser {
	classvar	updateProtos, searchMenuKeys, viewList, buttonSet, gray;

	var	<views,
			// historyPos == 0 means one item in history[0]
			// so it must be negative for empty history
		currentState, history, historyPos = -1,
		gui, hvBold12;

	*new { arg class;
		^super.new.init(class)
	}

		// some basic getters
		// views that can change current class or method
		// should always keep the state environment variables up-to-date
	current { ^currentState }
	currentClass { ^currentState.currentClass }
	currentMethod { ^currentState.currentMethod }

	init { arg class;
		this.class.initGUI;
		gui = GUI.scheme;
		hvBold12 = Font.sansSerif( 12 ).boldVariant;

		history = [];

		views = Environment(parent: updateProtos, know: true).make {

			~window = gui.window.new("class browser", Rect(128, (GUI.window.screenBounds.height - 638).clip(0, 320), 720, 600))
				.onClose_({ this.free });

			~window.view.decorator = FlowLayout(~window.view.bounds);

			~currentClassNameView = gui.textField.new(~window, Rect(0,0, 308, 32));
			~currentClassNameView.font = Font.sansSerif( 18 ).boldVariant;
			~currentClassNameView.align = \center;

			~currentClassNameView.action = {
				this.makeState(views.currentClassNameView.string.asSymbol.asClass, \class);
			};

			~superClassNameView = gui.staticText.new(~window, Rect(0,0, 256, 32));
			~superClassNameView.font = hvBold12 = Font.sansSerif( 12 ).boldVariant;

			~window.view.decorator.nextLine;

			~bakButton = gui.button.new(~window, Rect(0,0, 24, 24));
			~bakButton.states = [["<"]];
			~bakButton.action = {
				if (historyPos > 0) {
					historyPos = historyPos - 1;
					this.restoreHistory;
				}
			};

			~fwdButton = gui.button.new(~window, Rect(0,0, 24, 24));
			~fwdButton.states = [[">"]];
			~fwdButton.action = {
				if (historyPos < (history.size - 1)) {
					historyPos = historyPos + 1;
					this.restoreHistory;
				}
			};


			~superButton = gui.button.new(~window, Rect(0,0, 50, 24));
			~superButton.states = [["super"]];

			~superButton.action = {
				if(currentState.currentClass.notNil) {
					this.makeState( currentState.currentClass.superclass );
				}
			};

			~metaButton = gui.button.new(~window, Rect(0,0, 50, 24));
			~metaButton.states = [["meta"]];

			~metaButton.action = {
				if(currentState.currentClass.notNil) {
					this.makeState( currentState.currentClass.class );
				};
			};


			~helpButton = gui.button.new(~window, Rect(0,0, 50, 24));
			~helpButton.states = [["help"]];

			~helpButton.action = {
				if(currentState.currentClass.notNil) {
					currentState.currentClass.help;
				}
			};

			~classSourceButton = gui.button.new(~window, Rect(0,0, 90, 24));
			~classSourceButton.states = [["class source"]];

			~classSourceButton.action = {
				if(currentState.currentClass.notNil) {
					currentState.currentClass.openCodeFile;
				}
			};

			~methodSourceButton = gui.button.new(~window, Rect(0,0, 90, 24));
			~methodSourceButton.states = [["method source"]];
			~methodSourceButton.action = {
				if(currentState.currentMethod.notNil) {
					currentState.currentMethod.openCodeFile;
				};
			};

			~implementationButton = gui.button.new(~window, Rect(0,0, 100, 24));
			~implementationButton.states = [["implementations"]];
			~implementationButton.action = {
				if(currentState.currentMethod.notNil) {
					thisProcess.interpreter.cmdLine = currentState.currentMethod.name.asString;
					thisProcess.methodTemplates;
				};
			};

			~refsButton = gui.button.new(~window, Rect(0,0, 70, 24));
			~refsButton.states = [["references"]];
			~refsButton.action = {
				if(currentState.currentMethod.notNil) {
					thisProcess.interpreter.cmdLine = currentState.currentMethod.name.asString;
					thisProcess.methodReferences;
				};
			};

			~window.view.decorator.nextLine;

			GUI.staticText.new(~window, Rect(0, 0, 65, 20)).string_("Search for");
			~searchField = GUI.textField.new(~window, Rect(0, 0, 235, 20))
				.action_({
					this.searchClasses(views.searchField.string,
						views.searchMenu.value, views.matchCaseButton.value);
				});
			GUI.staticText.new(~window, Rect(0, 0, 15, 20)).string_("in").align_(\center);
			~searchMenu = GUI.popUpMenu.new(~window, Rect(0, 0, 200, 20));
			~matchCaseButton = GUI.button.new(~window, Rect(0, 0, 100, 20))
				.states_([["Case insensitive"], ["Match case"]]);

			~searchButton = GUI.button.new(~window, Rect(0, 0, 40, 20))
				.states_([["GO"]])
				.action_({
					this.searchClasses(views.searchField.string,
						views.searchMenu.value, views.matchCaseButton.value);
				});

			~window.view.decorator.nextLine;

			~filenameView = gui.staticText.new(~window, Rect(0,0, 600, 18));
			~filenameView.font = Font.sansSerif( 10 );

			~window.view.decorator.nextLine;
			gui.staticText.new(~window, Rect(0,0, 180, 24))
				.font_(hvBold12).align_(\center).string_("class vars");
			gui.staticText.new(~window, Rect(0,0, 180, 24))
				.font_(hvBold12).align_(\center).string_("instance vars");
			~window.view.decorator.nextLine;

			~classVarView = gui.listView.new(~window, Rect(0,0, 180, 130));
			~instVarView = gui.listView.new(~window, Rect(0,0, 180, 130));
			~classVarView.value = 0;
			~instVarView.value = 0;

			~window.view.decorator.nextLine;

			~subclassTitle = gui.staticText.new(~window, Rect(0,0, 220, 24))
				.font_(hvBold12).align_(\center).string_("subclasses  (press return)");
			~methodTitle = gui.staticText.new(~window, Rect(0,0, 240, 24))
				.font_(hvBold12).align_(\center).string_("methods");
			gui.staticText.new(~window, Rect(0,0, 200, 24))
				.font_(hvBold12).align_(\center).string_("arguments");
			~window.view.decorator.nextLine;

			~subclassView = gui.listView.new(~window, Rect(0,0, 220, 260));
			~methodView = gui.listView.new(~window, Rect(0,0, 240, 260));
			~argView = gui.listView.new(~window, Rect(0,0, 200, 260));
			~subclassView.resize = 4;
			~methodView.resize = 4;
			~argView.resize = 4;

			~window.view.decorator.nextLine;

			[~classVarView, ~instVarView, ~subclassView, ~methodView, ~argView].do (
				_.beginDragAction_({nil})
			);
		};

		this.addInstanceActions
			.makeState(class, \class);

		views.window.front;
	}

	makeState { |result, state = \class, addHistory = true, extraValuesDict|
		currentState = Environment(proto: views, parent: updateProtos, know: true).make {
			~result = result;
			if(~result.isNil or: { result.isArray and: { result.isEmpty } }) {
				~state = ("empty" ++ state).asSymbol
			} {
				~state = state;
			};
			if(extraValuesDict.respondsTo(\keysValuesDo)) {
				currentEnvironment.putAll(extraValuesDict);
			};
			~state.envirGet[\init].value;
			~subclassViewIndex = ~subclassViewIndex.value ? 0;
			~methodViewIndex = ~methodViewIndex.value ? 0;
			~argView.value = 0;
		};
		if (addHistory) {
			if (history.size > 1000) { history = history.drop(1) };
			historyPos = historyPos + 1;
			history = history.extend(historyPos).add(currentState);
		};
		this.updateViews;
	}

	restoreHistory {
			// note, don't use makeState because we already have the complete state saved
		currentState = history[historyPos];
		this.updateViews;
	}

	updateViews {
		var	updaters, bstate, view;
		currentState.use {
			updaters = ~state.envirGet;
			viewList.do { |viewname|
				updaters[viewname].value(viewname.envirGet);
			};
			buttonSet.do { |viewname|
				viewname.envirGet.tryPerform(\enabled_,
					updaters[\buttonsDisabled].includes(viewname).not);
			};
			~window.refresh;
		}
	}

	close {
		if(views.window.isClosed.not) { views.window.close };
		currentState = history = views = nil;		// dump garbage
	}

		// workaround for the fact that you can't get a non-metaclass from a metaclass
	*getClass { |method|
		var	class;
		if(method.isNil) { ^nil };
		^if((class = method.ownerClass).isMetaClass) {
			class.name.asString[5..].asSymbol.asClass;
		} { class }
	}

	getClass { |method| ^this.class.getClass(method) }

	searchClasses { |string, rootNumber, matchCase, addHistory = true|
		var	pool, rootdir, result,
			searchType = searchMenuKeys[rootNumber] ? \classSearch,
			isClassSearch;
		string = string ?? { "" };
		matchCase = matchCase > 0;
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
				pool = this.currentClass.allSubclasses ++ this.currentClass;
				if(this.currentClass.isMetaClass.not) {
					pool = pool ++ pool.collect(_.class);
				};
			}
			{ 4 } {
				isClassSearch = true;
				rootdir = PathName(currentState.currentClass.filenameSymbol.asString).pathOnly;
				pool = Class.allClasses.select({ |class|
					PathName(class.filenameSymbol.asString).pathOnly == rootdir
				});

				if(string.isEmpty) {
					this.makeState(
						pool.reject(_.isMetaClass).sort({ |a, b| a.name < b.name }),
						searchType
					);
					^this	// just to force early exit
				};
			};
		if (isClassSearch)
		{
			result = case (
				{string.isEmpty}, pool,
				{matchCase},
				{
					pool.select({ |class|
						class.isMetaClass.not and: { class.name.asString.contains(string) }
					})
				},
				//else
				{
					pool.select({ |class|
						class.isMetaClass.not and: { class.name.asString.containsi(string) }
					})
				}
			).sort({ |a, b| a.name < b.name });
		}
		{	// else - method search
			result = Array.new;
			case (
				{string.isEmpty}, {
					pool.do { |class|
						result = result.addAll(class.methods);
					}
				},
				{matchCase}, {
					pool.do({ |class|
						result = result.addAll(class.methods.select({ |method|
							method.name.asString.contains(string)
						}));
					});
				},
				{
					pool.do({ |class|
						result = result.addAll(class.methods.select({ |method|
							method.name.asString.containsi(string)
						}));
					});
				}
			);
			result = result.sort({ |a, b|
				if(a.name == b.name) { a.ownerClass.name < b.ownerClass.name }
					{ a.name < b.name };
			});
		};
		this.makeState(result, searchType, addHistory, extraValuesDict: (
			searchString: string,
			searchMenuValue: rootNumber,
			caseButtonValue: matchCase.binaryValue
		));
	}

	*initGUI {
		updateProtos ?? {
			gray = Color.grey(0.5);

			searchMenuKeys = #[classSearch, methodSearch, classSearch, methodSearch, classSearch];
			viewList = #[currentClassNameView, superClassNameView, filenameView,
				classVarView, instVarView, subclassTitle, methodTitle, subclassView,
				methodView, argView, searchMenu];
			buttonSet = IdentitySet[\superButton, \metaButton, \helpButton, \classSourceButton,
				\methodSourceButton, \implementationButton, \refsButton];
				// updateProtos holds instructions to update the GUI
				// for each kind of browser result to display
			updateProtos = (
				class: (
					init: {
						~setSubclassArray.value;
						~setMethodArray.value;
						~setCurrentClass.value(~result);
						~setCurrentMethod.value(~methodArray[0]);
					},
						// ~result must be a Class
					currentClassNameView: { |v| v.string = ~currentClass.name.asString },
					superClassNameView: { |v|
						v.string = "superclass: " ++ ~currentClass.superclass.tryPerform(\name).asString
					},
					filenameView: { |v| v.string = ~currentClass.filenameSymbol.asString },
					classVarView: { |v|
						v.items = ~currentClass.classVarNames.asArray.collectAs
							({|name| name.asString }, Array).sort
					},
					instVarView: { |v|
						v.items = ~currentClass.instVarNames.asArray.collectAs
							({|name| name.asString }, Array).sort
					},
					subclassTitle: { |v|
						v.string_("subclasses (press return)")
					},
					subclassView: { |v|
						~subclassView.items_(~subclassArray.collect({|class| class.name.asString }))
							.value_(~subclassViewIndex ? 0)
							.action_(~subclassViewNormalAction)
							.enterKeyAction_(~navigateToSubclassAction)
							.mouseDownAction_(~listViewDoubleClickAction);
					},
					methodTitle: { |v| v.string_("methods") },
					methodView: { |v|
						var colorFunc = { |class, name|
							if(class.findOverriddenMethod(name.asSymbol).isNil) {
								nil
							} {
								Color.grey(0.5, 0.8)
							}
						};
						var classMethodColors = ~classMethodNames.collect { |name|
							colorFunc.value(~currentClass.class, name)
						};
						var methodColors = ~methodNames.collect { |name|
							colorFunc.value(~currentClass, name)
						};
						var methodNames = ~classMethodNames ++ ~methodNames;
						var colors = classMethodColors ++ methodColors;

						~methodView.items_(methodNames)
							.value_(~methodViewIndex ? 0)
							.action_(~displayCurrentMethodArgsAction)
							.mouseDownAction_(~listViewDoubleClickAction)
							.tryPerform(\colors_, colors)
					},
					argView: { |v|
						if (~currentMethod.isNil or: { ~currentMethod.argNames.isNil }) {
							~argView.items = ["this"];
						} {
							~argView.items = ~currentMethod.argNames.collectAs( {|name, i|
								var defval;
								defval = ~currentMethod.prototypeFrame[i];
								if (defval.isNil) { name.asString }{
									name.asString ++ "     = " ++ defval.asString
								}
							}, Array);
						};
					},
					searchMenu: { |v|
						v.items = ["All classes",
							"All methods",
							"Subclasses of " ++ ~currentClass.name,
							"% + subclass methods".format(~currentClass.name),
							"Classes in folder "
								++ PathName(~currentClass.filenameSymbol.asString)
								.allFolders.last
						];
					},
					buttonsDisabled: IdentitySet.new	// all buttons OK
				),
					// this could happen if the user types a wrong class name into the class name box
				emptyclass: (
					currentClassNameView: { |v| v.string = "Class does not exist" },
					superClassNameView: { |v| v.string = "" },
					filenameView: { |v| v.string = "" },
					classVarView: { |v| v.items_(#[]) },
					instVarView: { |v| v.items_(#[]) },
					subclassTitle: { |v| v.string = "no subclasses" },
					methodTitle: { |v| v.string = "no methods" },
					subclassView: { |v| v.items_(#[]) },
					methodView: { |v| v.items_(#[]) },
					argView: { |v| v.items_(#[]) },
					buttonsDisabled: buttonSet	// all buttons disabled
				),
				classSearch: (
						// init: set current class and current method
					init: {
						~currentClass = ~result[0];
						~subclassArray = ~result;
						~setMethodArray.value(~currentClass);
						~currentMethod = ~methodArray[0];
					},
						// result must be an array of classes
					currentClassNameView: { |v| v.string = "Class Search: " ++ ~searchString },
					superClassNameView: { |v| v.string = "" },
					filenameView: { |v| ~class[\filenameView].value(v) },
					classVarView: { |v| ~class[\classVarView].value(v) },
					instVarView: { |v| ~class[\instVarView].value(v) },
					subclassTitle: { |v| v.string = "matching classes (press return)" },
					methodTitle: { |v| v.string = "methods" },
					subclassView: { |v|
						v.items_(~result.collect(_.name))
							.value_(~subclassViewIndex ? 0)
							.action_(~classSearchSubclassViewAction)
							.enterKeyAction_(~navigateToCurrentClassAction);
					},
					methodView: { |v| ~class[\methodView].value(v) },
					argView: { |v| ~class[\argView].value(v) },
					buttonsDisabled: #[]	// all buttons OK
				),
				emptyclassSearch: (
					currentClassNameView: { |v| v.string = "Class Search: " ++ ~searchString },
					superClassNameView: { |v| v.string = "No results found!" },
					filenameView: { |v| v.string = "" },
					classVarView: { |v| v.items_(#[]) },
					instVarView: { |v| v.items_(#[]) },
					subclassTitle: { |v| v.string = "no classes" },
					methodTitle: { |v| v.string = "no methods" },
					subclassView: { |v| v.items_(#[]) },
					methodView: { |v| v.items_(#[]) },
					argView: { |v| v.items_(#[]) },
					buttonsDisabled: buttonSet	// all buttons disabled
				),
				methodSearch: (
					init: {
						~methodArray = ~result;
						~currentMethod = ~methodArray[0];
						~methodViewIndex = 0;
						~setClassArrayFromMethodSearch.value(~result);
						~currentClass = this.getClass(~currentMethod);
						~subclassViewIndex = ~subclassArray.indexOf(~currentClass);
					},
						// result must be an array of methods
					currentClassNameView: { |v| v.string = "Method Search: " ++ ~searchString },
					superClassNameView: { |v| v.string = "" },
					filenameView: { |v| v.string = ~currentMethod.filenameSymbol },
					classVarView: { |v| ~class[\classVarView].value(v) },
					instVarView: { |v| ~class[\instVarView].value(v) },
					subclassTitle: { |v| v.string = "classes (press return)" },
					methodTitle: { |v| v.string = "matching methods (press return)" },
					subclassView: { |v|
						v.items_(~subclassArray.collect({ |class| class.name }))
							.value_(~subclassViewIndex ? 0)
							.action_(nil)
							.enterKeyAction_(~navigateToSubclassAction);
					},
					methodView: { |v|
						v.items_(~methodArray.collect({ |method|
							method.name ++ " (" ++ method.ownerClass ++ ")"
						})).action_(~methodSearchMethodViewAction)
						.value_(~methodViewIndex ? 0)
						.enterKeyAction_(~navigateToCurrentClassAction)
					},
					argView: { |v| ~class[\argView].value(v) },
					buttonsDisabled: #[]	// all buttons OK
				),
				emptymethodSearch: (
					currentClassNameView: { |v| v.string = "Method Search: " ++ ~searchString },
					superClassNameView: { |v| v.string = "No results found!" },
					filenameView: { |v| v.string = "" },
					classVarView: { |v| v.items_(#[]) },
					instVarView: { |v| v.items_(#[]) },
					subclassTitle: { |v| v.string = "no classes" },
					methodTitle: { |v| v.string = "no methods" },
					subclassView: { |v| v.items_(#[]) },
					methodView: { |v| v.items_(#[]) },
					argView: { |v| v.items_(#[]) },
					buttonsDisabled: buttonSet	// all buttons disabled
				),

					// support funcs shared in common
				setCurrentClass: { |class|
					~currentClass = class ?? { ~subclassArray[~subclassView.value ? 0] };
				},
				setCurrentMethod: { |method|
					~currentMethod = method ?? { ~methodArray[~methodView.value ? 0] };
				},
				setSubclassArray: { |class|
					class = class ? ~result;
					if (class.subclasses.isNil) { ~subclassArray = []; } {
						~subclassArray = class.subclasses.copy.sort {|a,b| a.name <= b.name };
					};
				},
				setMethodArray: { |class|
					class = class ? ~result;
					if (class.class.methods.isNil) {
						~classMethodArray = [];
						~classMethodNames = [];
					} {
						~classMethodArray = class.class.methods.asArray.copy.sort
											{|a,b| a.name <= b.name };
						~classMethodNames = ~classMethodArray.collect {|method|
							"*" ++ method.name.asString
						};
					};
					if (class.methods.isNil) {
						~methodArray = [];
						~methodNames = [];
					} {
						~methodArray = class.methods.asArray.copy.sort
											{|a,b| a.name <= b.name };
						~methodNames = ~methodArray.collect {|method|
							method.name.asString
						};
					};
					~methodArray = ~classMethodArray ++ ~methodArray;
				},
				setClassArrayFromMethodSearch: { |result|
					var	classSet = IdentitySet.new, class;
					~result.do({ |method|
						classSet.add(this.getClass(method));
					});
					~subclassArray = classSet.asArray.sort({ |a, b| a.name < b.name });
				}
			);
		}
	}

		// because these use instance variables, they cannot be defined in the above class method
		// each ClassBrowser instance gets a separate dictionary with the proper object scope
	addInstanceActions {
		views.putAll((
				// these are called from GUI, cannot assume currentState is already in force
			navigateToSubclassAction: {
				var	prevState = currentState, prevMethod = prevState.currentMethod;
				this.makeState(currentState.subclassArray[currentState.subclassView.value], \class,
						// methodViewIndex func runs in context
						// of the new state environment
						// might be nil unless subclass also implements
					extraValuesDict: (methodViewIndex: {
						if(prevMethod.isNil) { 0 } {
							~methodArray.detectIndex({ |item|
								item.name == prevMethod.name
							})
						};
					}));
			},
			navigateToCurrentClassAction: {
				var	prevState = currentState;
				this.makeState(currentState.currentClass, \class,
					extraValuesDict: (methodViewIndex: {
						~methodArray.indexOf(prevState.currentMethod)
					}));
			},
			subclassViewNormalAction: { |view|
				currentState.subclassViewIndex = view.value;
			},
			displayCurrentMethodArgsAction: { |view|
				currentState.use {
					~methodViewIndex = view.value;
					~setCurrentMethod.value;
					~class[\argView].value(~argView)
				}
			},
			methodEnterKeyAction: {
				currentState.currentMethod !? { currentState.currentMethod.openCodeFile };
			},
			classSearchSubclassViewAction: { |view|
				currentState.use {
					~subclassViewIndex = view.value;
					~setCurrentClass.value;
					~setMethodArray.value(~currentClass);
					~methodViewIndex = 0;
					~class[\filenameView].value(~filenameView);
					~class[\methodView].value(~methodView);
					~class[\argView].value(~argView);
					~class[\classVarView].value(~classVarView);
					~class[\instVarView].value(~instVarView);
				}
			},
			methodSearchMethodViewAction: { |view|
				currentState.use {
					var	index;
					~methodViewIndex = view.value;
					~currentMethod = ~methodArray[~methodView.value ? 0];
					~currentClass = this.getClass(~currentMethod);
					~methodSearch[\filenameView].value(~filenameView);
					~class[\argView].value(~argView);
					~class[\classVarView].value(~classVarView);
					~class[\instVarView].value(~instVarView);
					if((index = ~subclassArray.indexOf(~currentClass)).notNil) {
						~subclassView.value = index;
						~subclassViewIndex = index;
					};
				};
			},
			listViewDoubleClickAction: { |view, x, y, mods, buttonNumber, clickCount|
				if(clickCount == 2) { view.enterKeyAction.value }
			}
		));
	}
}
