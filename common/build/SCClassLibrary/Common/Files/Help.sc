/*
Help.tree and Help.gui - a scheme to allow UGens, no wait I mean ALL classes,
to be "self-classified" and provide a pleasant-ish browsing interface. No wait,
let's put all help docs into the tree too! Yeah!

By Dan Stowell, 2007
with lots of input from Scott Wilson
and crossplatform tips from nescivi

Try it:
Help.gui

Help.dumpTree

see also:
Class.browse
*/

Help {
	classvar <global, categoriesSkipThese;
	classvar <filterUserDirEntries, <>cachePath;

	var	tree, fileslist, <root;

	*initClass {
		var	dir;
		cachePath = Platform.userAppSupportDir +/+ "SC_helptree.cache.txt";
		categoriesSkipThese = [Filter, BufInfoUGenBase, InfoUGenBase, MulAdd, BinaryOpUGen,
						UnaryOpUGen, BasicOpUGen, LagControl, TrigControl, MultiOutUGen, ChaosGen,
			Control, OutputProxy, AbstractOut, AbstractIn, Object, Class];
		if(\SCImageFilter.asClass.notNil) {
			categoriesSkipThese = categoriesSkipThese.add(\SCImageFilter.asClass)
		};
		filterUserDirEntries = [ "Extensions", "SuperCollider", "SuperCollider3", "Help", "svn", "share", "classes", "trunk", "Downloads", "Application Support" ];
		[Platform.systemExtensionDir, Platform.userExtensionDir].do({ |path|
			path.split(Platform.pathSeparator).do({ |dirname|
				if(filterUserDirEntries.detect(_ == dirname).isNil) {
					filterUserDirEntries = filterUserDirEntries.add(dirname);
				};
			});
		});
		global = this.new;
	}

	*new { |root|
		if(root.isNil and: { global.notNil }) {
			^global
		} {
			^super.new.init(root)
		}
	}

	init { |rootdir|
		root = rootdir !? { rootdir.absolutePath };
		if(root.isNil or: { root.size == 0 }) {
			this.tree;
		} {
			this.tree(false, false, false, root, false);
		}
	}

	tree { |sysext=true, userext=true, allowCached=true, root, writeCache=true|
		var classes, node, subc, helpRootLen;
		var helpExtensions = ['html', 'htm', 'scd', 'rtf', 'rtfd'];
		var helpDirs = Array.new;
		var thisHelpExt;
		if(allowCached and: {tree.isNil and: {File.exists(this.cachePath)}}){
			"Help tree read from cache in % seconds".format({
				this.readTextArchive; // Attempt to read cached version
			}.bench(false)).postln;
		};
		if(tree.isNil, { "Help files scanned in % seconds".format({
			// Building the tree

			// Help file paths - will be used for categorising, if categories is nil or if not a class's helpfile.
			// Otherwise they'll be stored for quick access to helpfile.
			fileslist = IdentityDictionary.new;
			if(root.isNil) {
				helpDirs = helpDirs.add( Platform.helpDir );
			} {
				helpDirs = helpDirs.add(root);
			};
			if ( sysext ,{
				helpDirs = helpDirs.add( Platform.systemExtensionDir );
			});
			if ( userext ,{
				helpDirs = helpDirs.add( Platform.userExtensionDir );
			});

			// Now check each class's ".categories" response
			classes = Object.allSubclasses.difference(categoriesSkipThese).reject({|c| c.asString.beginsWith("Meta_")});
			if(root.notNil) {
					// consider only classes whose main definition is in the root folder
				classes = classes.select({ |c| c.filenameSymbol.asString.beginsWith(root) });
			};
			tree = Dictionary.new(8);
			classes.do({|class| this.addCatsToTree(class, fileslist)});

			// Now add the remaining ones to the tree - they're everything except the classes which
			//      have declared their own categorisation(s).

			helpDirs.do{ |helpDir|
				this.addDirTree( helpDir,tree );
			};
			if(writeCache) {
				{
					this.writeTextArchive;
				}.defer(4.312);
				{
					var sclang_completion_dict, dictfile;
					// Also piggyback on the tree struct to write out an auto-completion dictionary:
					sclang_completion_dict = SortedList.new;
					this.do{|key, value|
						if("0123456789".includes(key[0]).not and:{key.includes($ ).not}){
							sclang_completion_dict.add(key);
						}
					};
					dictfile = File(Platform.userAppSupportDir +/+ "sclang_completion_dict", "w");
					if(dictfile.isOpen){
						sclang_completion_dict.do{|entry| dictfile.write(entry ++ Char.nl)};
						dictfile.close;
					};
				}.defer(7.1101);
			};
		}.bench(false)).postln});
		^tree;
	}

	findKeysForValue{|val|
		var func, node, keyPath;
		keyPath =[];
		if( val.beginsWith( "SC://" ), { ^keyPath });
		// only file scheme urls in tree
		if( val.beginsWith( "file:" ), {
			val = val.copyToEnd( if( val.copyRange( 5, 6 ) == "//", 7, 5 ));
		});
		val = val.replace("%20", " ");
		func = {|dict, depth = 0|
			node = dict.findKeyForValue(val);
			node.isNil.if({
				dict.keysValuesDo({|key, item|
					item.isKindOf(Dictionary).if({
						keyPath = keyPath.copyFromStart(depth - 1).add(key);
						func.value(item, depth + 1)
					})
				});
			}, {^keyPath.add(node)});
		};
		func.value(this.tree);
		^[];
	}

	addUserFilter{ |subpath|
		filterUserDirEntries = filterUserDirEntries.add( subpath );
		this.forgetTree;
	}

	addDirTree{ |helppath,tree|
		var helpExtensions = #['html', 'htm', 'scd', 'rtf', 'rtfd'];
		var subfileslist;
		var node, subc, helpRootLen, thisHelpExt;

		subfileslist = IdentityDictionary.new;

		PathName.new(helppath.standardizePath).helpFilesDo({|pathname|
				if( pathname.fullPath.contains("ignore")){
					"Not ignoring: %".format(pathname.fullPath).postln;
				};
				if( pathname.fullPath.contains("3vs2").not
					and: { pathname.fullPath.contains("help-scripts").not }
					, {
						subfileslist[pathname.fileNameWithoutDoubleExtension.asSymbol] = pathname.fullPath;
						fileslist[pathname.fileNameWithoutDoubleExtension.asSymbol] = pathname.fullPath;
					})
			});

		helpRootLen = (helppath.standardizePath).size + 1;
		subfileslist.keysValuesDo({ |classsym, path|

			subc = path[helpRootLen..].split(Platform.pathSeparator);
			if ( helppath == Platform.helpDir,
				{
					subc = subc[0..subc.size-2]; // Ignore "Help" and the filename at the end
//					subc = path[helpRootLen..].split(Platform.pathSeparator);
				},{
					//helpRootLen = "~".standardizePath;
					if ( helppath == Platform.systemExtensionDir,
						{
//							subc = path[helpRootLen..].split(Platform.pathSeparator);
							subc = [ "SystemExtensions" ] ++ subc;
							//subc.postcs;
						});
					if ( helppath == Platform.userExtensionDir,
						{
							helpRootLen = "~/".absolutePath.size; // + 1;
							subc = path[helpRootLen..].split(Platform.pathSeparator);
							subc = [ "UserExtensions" ] ++ subc;
							// check for common superfluous names that may confuse the categorisation;
							filterUserDirEntries.do{ |spath|
								subc = subc.reject{ |it|
									it == spath;
								};
							};
							// check for double entries (e.g. SwingOSC)
							subc[..subc.size-2].do{ |it,i|
								var subset;
								subset = subc[..i-1];
								if ( subset.detect( { |jt| jt == it } ).size > 0, {
									subc = subc[..i-1] ++ subc[i+1..];
								});
							};
						});
					if ( helppath == root,
						{
								// exactly the same as the previous if,
								// but it isn't right to change helpRootLen
							// check for common superfluous names that may confuse the categorisation;
							filterUserDirEntries.do{ |spath|
								subc = subc.reject{ |it|
									it == spath;
								};
							};
							// check for double entries (e.g. SwingOSC)
							subc[..subc.size-2].do{ |it,i|
								var subset;
								subset = subc[..i-1];
								if ( subset.detect( { |jt| jt == it } ).size > 0, {
									subc = subc[..i-1] ++ subc[i+1..];
								});
							};
						});
					subc = subc[..subc.size-2];
				}
			);
			if(subc.size > 0) {
				thisHelpExt = helpExtensions.select{ |ext|
					subc.last.endsWith("."++ext)
				};
			};
			if ( thisHelpExt.size > 0 , {
				subc = subc[..subc.size-2];
			});

			subc = subc.collect({|i| "[["++i++"]]"});
			node = tree;
			// Crawl up the tree, creating hierarchy as needed
			subc.do({|catname|
				if(node[catname].isNil, {
					node[catname] = Dictionary.new(3);
				});
				node = node[catname];
			});
			// "node" should now be the tiniest branch
			node[classsym.asClass ? classsym] = path;
		});
	}

	rebuildTree {
		this.forgetTree;
		if(root.isNil) {
			this.tree(allowCached:false);
		} {
			this.tree(false, false, false, root, false);
		}
	}

	forgetTree {
		tree = nil;
	}

	dumpTree { |node, prefix=""|
		node = node ?? {this.tree};
		node.keysValuesDo({ |key, val|
			if(val.isKindOf(Dictionary), {
				(prefix + key).postln;
				this.dumpTree(val, prefix ++ "   ");
			}, {
				(prefix + key ++":" + val).postln;
			});
		});
	}

	addCatsToTree { |class, fileslist|
		var subc, node;

		if(class.categories.isNil.not, {
			class.categories.do({|cat|
				subc = cat.split($>).collect({|i| "[["++i++"]]"});
				node = tree;
				// Crawl up the tree, creating hierarchy as needed
				subc.do({|catname|
					if(node[catname].isNil, {
						node[catname] = Dictionary.new(3);
					});
					node = node[catname];
				});
				// "node" should now be the tiniest branch
				node[class] = fileslist[class.asSymbol] ? "";
			});

			// Class has been added to list so we're OK
			fileslist.removeAt(class.asSymbol);
		}); // end if

	}

	writeTextArchive{ |path|
		var fp;
		if(path.isNil){ path = this.cachePath };
		fp = File(path, "w");
		this.prRecurseTreeToFile(fp, this.tree); // Must use this.tree - will create if not exists
		fp.write(fileslist.asTextArchive);
		fp.close;
	}

	prRecurseTreeToFile{ | fp, treeBit, numtabs=0 |
		fp.write("%%\n".format($\t.dup(numtabs).join, treeBit.size));
		treeBit.keysValuesDo{| key, val |
			fp.write("%%\n".format($\t.dup(numtabs).join, key.asString));
			if(val.isKindOf(Dictionary)){
				this.prRecurseTreeToFile(fp, val, numtabs + 1)
			}{
				fp.write("%%\n".format($\t.dup(numtabs).join, val.asString));
			}
		};
	}

	readTextArchive{ |path|
		var fp, filesliststr;
		if(path.isNil){ path = this.cachePath };
		fp = File(path, "r");
		try{
			tree = this.prRecurseTreeFromFile(fp);
		}{
			"Failure to read tree in Help.treeFromFile(%)".format(path).warn;
			this.forgetTree;
		};
		try{
			filesliststr = fp.readAllString;
			if(filesliststr.size < 10){ // Old version of cache didn't store it...
				"Help.tree cache has no fileslisttree. Will regenerate it".warn;
				this.forgetTree;
			}{
				fileslist = filesliststr.interpret;
			};
		}{
			"Failure to read fileslist in Help.treeFromFile(%)".format(path).warn;
			this.forgetTree;
		};
		fp.close;
	}

	prRecurseTreeFromFile{ | fp, numtabs=0 |
		var line, numentries, dict, key;
		line = fp.getLine[numtabs..];
		numentries = line.asInteger;
		dict = Dictionary.new(numentries);
		numentries.do{
			line = fp.getLine[numtabs..];
			key = line;
			if(key[0]==$[ and:{ key[1]==$[ }){
				// starting with [[ is indicator of being a category
				dict[key] = this.prRecurseTreeFromFile(fp, numtabs+1);
			}{
				line = fp.getLine[numtabs..];
				key = key.asSymbol;
				key = key.asClass ? key; // Classes become classes, topics stay as symbols
				dict[key] = line;
			}
		};
		^dict
	}

gui { |sysext=true,userext=true, allowCached=true|
	var classes, win, lists, listviews, numcols=7, selecteditem, node, newlist, curkey;
	var selectednodes, scrollView, compView, textView, keys;
	var classButt, browseButt, bwdButt, fwdButt;
	var isClass, history = [], historyIdx = 0, fBwdFwd, fHistoryDo, fHistoryMove;
	var screenBounds, bounds, textViewBounds, results, resultsview, statictextloc;
	var searchField, helpguikeyacts, fSelectTreePath, inPathSelect = false, fUpdateWinTitle, fLoadError;

	// Call to ensure the tree has been built
	this.tree( sysext, userext, allowCached );

	// Now for a GUI
	screenBounds = Window.screenBounds;
	bounds = Rect(128, 264, 1040, 564);
	bounds = bounds.center_(screenBounds.center);
	bounds = bounds.sect(screenBounds.insetBy(15));
	win = Window.new("Help browser", bounds); // SCWindow
	// scrollView and compView hold the category-browsing list widgets
	scrollView = ScrollView.new(win, Rect(5, 0, 425, 529)).hasBorder_(true).resize_(4);
	compView = CompositeView.new(scrollView, Rect(0, 0, numcols * 200, /*504*/ bounds.height-60));
	// textView displays a help file "inline"
	textViewBounds = Rect(435, 0, /*620*/bounds.width-435, /*554*/ bounds.height-35);
	textView = TextView.new(win, textViewBounds)
		.hasVerticalScroller_(true)
		.hasHorizontalScroller_(false)
		.autohidesScrollers_(false)
		.resize_(5)
		.canFocus_(true);

	if(GUI.current.id == \swing, { textView.editable_( false ).canFocus_( true ) });

	textView.bounds = textView.bounds; // hack to fix origin on first load

	// hidden at first, this will receive search results when the search field is activated
	resultsview = ScrollView(win, textViewBounds)
				.resize_(5)
				.visible_(false);

	// updates the history arrow buttons
	fBwdFwd = {
		bwdButt.enabled = historyIdx > 0;
		fwdButt.enabled = historyIdx < (history.size -	1);
	};

	fLoadError = { |error|
		error.reportError;
		"\n\nA discrepancy was found in the help tree.".postln;
		if(allowCached) {
			"rm \"%\"".format(cachePath).unixCmd;
			"The help tree cache may be out of sync with the file system. Rebuilding cache. Please reopen the Help GUI when this is finished.".postln;
			this.rebuildTree;
			win.close;
		} {
			"Please report the above error dump on the sc-users mailing list.".postln;
		};
	};

	// cuts the redo history, adds and performs a new text open action
	fHistoryDo = { arg selector, argum;
		history		= history.copyFromStart( historyIdx ).add([ selector, argum ]);
		historyIdx	= history.size - 1;
		try({ textView.perform( selector, argum ) }, fLoadError);
		fBwdFwd.value;
	};

	// moves relatively in the history, and performs text open action
	fHistoryMove = { arg incr; var entry;
		historyIdx	= historyIdx + incr;
		entry		= history[ historyIdx ];
		try({ textView.perform( entry[ 0 ], entry[ 1 ]) }, fLoadError);
		fBwdFwd.value;
	};

	// keep this check for compatibility with old versions of swingOSC
	if( textView.respondsTo( \linkAction ), {
		textView
			.linkAction_({ arg view, url, descr;
				var path;
				if( url.notEmpty, {
					//fHistoryDo.value( \open, url );
					keys = this.findKeysForValue(url);
					if(keys.size == 0, {
						("Invalid hyperlink:" + url + "Please repair this.\nSearching help directories for alternative.").warn;
						url = Help.findHelpFile(url.basename.splitext.first);
						url.notNil.if({keys = this.findKeysForValue(url)});
					});
					if(keys.size > 0, {
						fSelectTreePath.value(keys.drop(-1), keys.last.asString);
					});
				}, {
					if( descr.beginsWith( "SC://" ), {
						fHistoryDo.value( \open, descr );
					});
				});
			});
	});

	lists = Array.newClear(numcols);
	lists[0] = tree.keys(Array).collect(_.asString).sort;
	selectednodes = Array.newClear(numcols);

	// SCListView
	listviews = (0..numcols-1).collect({ arg index; var view;
		view = ListView( compView, Rect( 5 + (index * 200), 4, 190, /* 504 */ bounds.height - 60 ));
		//view.items = []; // trick me into drawing correctly in scrollview
		if( view.respondsTo( \allowsDeselection ), {
			view.allowsDeselection_( true ).value_( nil );
		});
		view
		.resize_(4)
		.action_({ arg lv; var lv2;
			if( lv.value.notNil, {
				// We've clicked on a category or on a class

				if((lv.items.size != 0), {
					lv2 = if( index < (listviews.size - 1), { listviews[ index + 1 ]});

					selecteditem = lists[index][lv.value];
					if( lv2.notNil, {
						// Clear the GUI for the subsequent panels
						listviews[index+1..].do({ arg lv; lv.items=#[];
							if( lv.respondsTo( \allowsDeselection ), { lv.value = nil })});
					});

					// Get the current node, from the parent node
					node = try { if(index==0, tree, {selectednodes[index-1]})[selecteditem] };
					curkey = selecteditem;
					selectednodes[index] = node;

					if(node.isNil, {
						// We have a "leaf" (class or helpdoc), since no keys found

						if( (index + 1 < lists.size), { lists[index+1] = #[] });

						if(inPathSelect.not, {
						{
							// Note: the "isClosed" check is to prevent errors caused by event triggering while user closing window
							if(textView.isClosed.not){textView.visible = true};
							if(resultsview.isClosed.not){resultsview.visible = false};
							fHistoryDo.value( \open, fileslist.at( selecteditem.asSymbol ) ? fileslist.at( \Help ));
						}.defer( 0.001 );
						});
						isClass = selecteditem.asSymbol.asClass.notNil;
						// Note: "Help" class is not the class that matches "Help.html", so avoid potential confusion via special case
                            if(classButt.notNil){
                            	classButt.enabled_((selecteditem!="Help") and: {isClass});
                            };
						browseButt.enabled_((selecteditem!="Help") and: {isClass});
						// The "selectednodes" entry for the leaf, is the path to the helpfile (or "")
						selectednodes[index] = try { if(index==0, {tree}, {selectednodes[index-1]})
									[curkey.asSymbol.asClass ? curkey.asSymbol]};

						fUpdateWinTitle.value;
					}, {
						// We have a category on our hands
						if( lv2.notNil, {
							lists[ index + 1 ] = node.keys(Array).collect(_.asString).sort({|a,b|
									// the outcomes:
									// a and b both start with open-bracket:
									//	test result should be a < b
									// or one starts with open-bracket and the other doesn't (xor)
									//	test result should be whether it's a that has the bracket
								if(a[0] == $[ /*]*/ xor: (b[0] == $[ /*]*/)) {
									a[0] == $[ /*]*/
								} {
									a < b
								}
							});
							lv2.items = lists[index+1];
						});

					});

					if( (index + 1) < listviews.size, {
						listviews[index+1].value = if( listviews[index+1].respondsTo( \allowsDeselection ).not, 1 );
						listviews[index+1].valueAction_( 0 );
					});
					selectednodes[index+2 ..] = nil; // Clear out the now-unselected
				});
			});
		});
	});

	listviews[0].items = lists[0];

	// Add keyboard navigation between columns
	listviews.do({ |lv, index| // SCView
		lv.keyDownAction_({|view,char,modifiers,unicode,keycode|
			var nowFocused, lv2;
			nowFocused = lv;
			switch(unicode,
			// cursor left
			63234, { if(index > 0, { lv2 = listviews[ index - 1 ]; lv2.focus; nowFocused = lv2 })
			},
			// cursor right
			63235, { if( index < (listviews.size - 1) and: { listviews[ index + 1 ].items.notNil }, {
						lv2 = listviews[ index + 1 ];
						try {
							lv2.value_( if( lv2.respondsTo( \allowsDeselection ).not, - 1 )).valueAction_( 0 ).focus;
							nowFocused = lv2;
						}
				   })
			},
			13, { // Hit RETURN to open source or helpfile
				// The class name, or helpfile name we're after

				if(lv.value.notNil and: {if(index==0, tree, {selectednodes[index-1]})[lists[index][lv.value]].isNil}, {
					{ selecteditem.openHelpFile }.defer;
				});
			},
			//default:
			{
				// Returning nil is supposed to be sufficient to trigger the default action,
				// but on my SC this doesn't happen.
				view.defaultKeyDownAction(char,modifiers,unicode);
			});
			if(scrollView.visibleOrigin.x > nowFocused.bounds.left or: {scrollView.visibleOrigin.x + scrollView.bounds.width > nowFocused.bounds.left}, {
				scrollView.visibleOrigin_(Point(nowFocused.bounds.left - 5, 0));
			});
		})
		.mouseDownAction_({|view, x, y, modifiers, buttonNumber, clickCount|
			{
			if(lists[index][lv.value][0]==$[, {
				if(scrollView.visibleOrigin.x != (lv.bounds.left - 5), {
					{
					10.do({|i| { scrollView.visibleOrigin_(
									Point(((lv.bounds.left - lv.bounds.width)+((10+i)*10)-5), 0))
								}.defer;
						0.02.wait;
					});
					}.fork;
				});
			});
			}.defer(0.01); // defer because .action above needs to register the new index

			if(clickCount == 2, {
				if(lv.value.notNil and: { try { if(index==0, tree, {selectednodes[index-1]})[lists[index][lv.value]] }.isNil}, {
					{ selecteditem.openHelpFile }.defer;
				});
			});
		});
	});

	// Add ability to programmatically select an item in a tree
	fSelectTreePath = { | catpath, leaf |
		var foundIndex;
		Task{
			0.001.wait;
			inPathSelect = true;
			catpath.do{ |item, index|
				foundIndex = listviews[index].items.indexOfEqual(item);
				if(foundIndex.notNil){
					listviews[index].value_(foundIndex).doAction;
				}{
					"Could not select menu list item % in %".format(item, listviews[index].items).postln;
				};
				0.02.wait;
			};
			inPathSelect = false;
			foundIndex = listviews[catpath.size].items.indexOfEqual(leaf);
			if(foundIndex.notNil){
				listviews[catpath.size].value_(foundIndex).doAction;
//				history = history.drop(-1);
//				historyIdx = history.size - 1;
			}{
				"Could not select menu list item %".format(leaf).postln;
			};
			textView.visible = true;
			resultsview.visible = false;
			fUpdateWinTitle.value;
			win.front;
		}.play(AppClock);
	};

	fUpdateWinTitle = {
		win.name_(
			(["Help browser"] ++ listviews.collect{|lv| lv.value !? {lv.items[lv.value]} }.reject(_.isNil)).join(" > ") );
	};

	Platform.case(\windows, {
            // TEMPORARY WORKAROUND:
            // At present, opening text windows from GUI code can cause crashes on Psycollider
            // (thread safety issue?). To work around this we just remove those buttons.
	}, {
		Button.new( win, Rect( 5, /* 534 */ bounds.height - 30, 110, 20 ))
			.states_([["Open Help File", Color.black, Color.clear]])
			.resize_(7)
			.action_({{ selecteditem.openHelpFile }.defer;});
		classButt = Button.new( win, Rect( 119, /* 534 */ bounds.height - 30, 110, 20 ))
			.states_([["Open Class File", Color.black, Color.clear]])
			.resize_(7)
			.action_({
				if(selecteditem.asSymbol.asClass.notNil, {
					{selecteditem.asSymbol.asClass.openCodeFile }.defer;
				});
			});
	});
	browseButt = Button.new( win, Rect( 233, /* 534 */ bounds.height - 30, 110, 20 ))
		.states_([["Browse Class", Color.black, Color.clear]])
		.resize_(7)
		.action_({
			if(selecteditem.asSymbol.asClass.notNil, {
				{selecteditem.asSymbol.asClass.browse }.defer;
			});
		});
	bwdButt = Button.new( win, Rect( 347, /* 534 */ bounds.height - 30, 30, 20 ))
		.states_([[ "<" ]])
		.resize_(7)
		.action_({
			if( historyIdx > 0, {
				fHistoryMove.value( -1 );
			});
		});
	fwdButt = Button.new( win, Rect( 380, /* 534 */ bounds.height - 30, 30, 20 ))
		.states_([[ ">" ]])
		.resize_(7)
		.action_({
			if( historyIdx < (history.size - 1), {
				fHistoryMove.value( 1 );
			});
		});
	fBwdFwd.value;

	// textfield for searching:
	statictextloc = Rect(10, 10, textViewBounds.width-20, 200);
	StaticText.new(win, Rect(435, bounds.height-35, 100 /* bounds.width-435 */, 35))
		.align_(\right).resize_(7).string_("Search help files:");
	searchField = TextField.new(win, Rect(535, bounds.height-35, bounds.width-535-35, 35).insetBy(8))
		.resize_(8).action_({|widget|

			if(widget.value != ""){
				// Let's search!
				// hide the textView, show the resultsview, do a query
				textView.visible = false;
				resultsview.visible = true;
				resultsview.removeAll;
				results = this.search(widget.value);
				// Now add the results!
				StaticText(resultsview, Rect(0, 0, textViewBounds.width / 2, 30))
					.resize_(1)
					.align_(\right)
					.string_("% results found for query '%'.".format(results.size, widget.value));
				Button(resultsview, Rect(textViewBounds.width / 2, 0, 100, 30).insetBy(5))
					.resize_(1)
					.states_([["Clear"]])
					.action_({ searchField.valueAction_("") })
					.focus();
				results.do{|res, index|
					res.drawRow(resultsview, Rect(0, index*30 + 30, textViewBounds.width, 30),
						// Add an action that uses the gui itself:
						{ fSelectTreePath.(res.catpath, res.docname) }
						);
				};

			}{
				// Empty query string, go back to textView
				textView.visible = true;
				resultsview.visible = false;
			};

		});

	// Handle some "global" (for the Help gui) key actions
	helpguikeyacts = {|view, char, modifiers, unicode, keycode|
		if((modifiers & (262144 | 1048576)) != 0){ // cmd or control key is pressed
			unicode.switch(
				6, { // f for find
					searchField.focus;
				},
				8, // h for home
				{
					{
						listviews[0].valueAction_(listviews[0].items.find(["Help"]));
						scrollView.visibleOrigin_(0@0);
					}.defer(0.001)
				}
			);
		};
	};
	win.view.addAction(helpguikeyacts, \keyUpAction);

	win.onClose_{
		// This is done to prevent Cmd+W winclose from trying to do things in vanishing textviews!
		fHistoryDo = {};
	};

	win.front;
	listviews[0].focus;
	if(listviews[0].items.detect({ |item| item == "Help" }).notNil) {
		fSelectTreePath.([], "Help"); // Select the "Help" entry in the root
		selecteditem = "Help";
	} {
		selecteditem = listviews[0].items.first;
		fSelectTreePath.([], selecteditem);
	}
}
// end gui

	all {
		//		^this.new("Help/").dumpToDoc("all-helpfiles");
		var doc;
		var helpExtensions = ['html', 'htm', 'scd', 'rtf', 'rtfd'];
		var str = CollStream.new;
		doc = Document.new("all-helpfiles");
		[       Platform.helpDir,
			Platform.systemExtensionDir,
			Platform.userExtensionDir
		].do{ |it|
			PathName.new( it ).foldersWithoutSVN.do{ |folderPn|
				str << folderPn.fullPath << Char.nl;
				folderPn.helpFilesDo { |filePn|
					str << Char.tab <<
					filePn.fileNameWithoutExtension  << Char.nl;
				};
			}
		};
		doc.string = str.collection;
	}

	// Iterates the tree, finding the help-doc paths and calling action.value(docname, path)
	do { |action|
		this.pr_do(action, this.tree, []);
	}
	pr_do { |action, curdict, catpath|
		curdict.keysValuesDo{|key, val|
			if(val.class == Dictionary){
				this.pr_do(action, val, catpath ++ [key]) // recurse
			}{
				action.value(key.asString, val, catpath)
			}
		}
	}

	searchGUI {
		this.deprecated(thisMethod, Meta_Help.findRespondingMethodFor(\gui));
		^this.gui
	}

	// Returns an array of hits as HelpSearchResult instances
	search { |query, ignoreCase=true|
		var results = List.new, file, ext, docstr, pos;
		this.do{ |docname, path, catpath|
			if(path != ""){
				if(docname.find(query, ignoreCase).notNil){
					results.add(HelpSearchResult(docname, path, 100 / (docname.size - query.size + 1), "", catpath.deepCopy));
				}{
					ext = path.splitext[1];
					// OK, let's open the document, see if it contains the string... HEAVY!
					file = File(path, "r");
					if(file.isOpen){
						docstr = ext.switch(
							"html", {file.readAllStringHTML},
							"htm",  {file.readAllStringHTML},
							"rtf",  {file.readAllStringRTF},
							        {file.readAllString}
							);
						file.close;
						pos = docstr.findAll(query, ignoreCase);
						if(pos.notNil){
							results.add(HelpSearchResult(docname, path, pos.size, docstr[pos[0] ..  pos[0]+50], catpath.deepCopy));
						}
					}{
						"Help.search will rebuild help cache, since an expected file was not found: %".format(path).postln;
						// EXPERIMENTAL:
						// File:isOpen failure means that the help tree cache is out of date.
						// So let's explicitly destroy and recreate it.
						// There may be a danger of infinite loops if some weird filesystem stuff is happening...?
						this.forgetTree;
						this.tree(allowCached: false);
						^this.search(query, ignoreCase);
					}
				}
			}{
				//"empty path: %".format(docname).postln;
			}
		};
		results = results.sort;

		^results
	}
	// This iterates the Help.tree to find the file. Can be used instead of platform-specific approaches
	findHelpFile { |str|
		str = str.asString;
		this.do { |key, path|
			if(key == str and: { path.size > 0 }) { ^path }
		};
		^nil
	}

	// does the same as findHelpFile, but replaces the string with "Help" if the string is empty. This makes it possible in sced to open the main help if nothing is selected.
	findHelpFileOrElse { |str|
		str = str.asString;
		if ( str.isEmpty ) { str = "Help" };
		^Help.findHelpFile( str );
	}

// class method interface

	*tree { |sysext = true, userext = true, allowCached = true| ^global.tree(sysext, userext, allowCached) }
	*addUserFilter { |subpath| ^global.addUserFilter(subpath) }
	*addDirTree { |helppath, tree| ^global.addDirTree(helppath, tree) }
	*rebuildTree { ^global.rebuildTree }
	*forgetTree { ^global.forgetTree }
	*dumpTree { |node, prefix = ""| ^global.dumpTree(node, prefix) }
	*addCatsToTree { |class, fileslist| ^global.addCatsToTree(class, fileslist) }
	*gui { |sysext = true, userext = true| ^global.gui(sysext, userext) }
	*all { ^global.all }
	*do { |action| ^global.do(action) }
	*pr_do { |action, curdict| ^global.pr_do(action, curdict) }
	*searchGUI { ^global.searchGUI }
	*search { |query, ignoreCase| ^global.search(query, ignoreCase) }
	*findHelpFile { |str| ^global.findHelpFile(str) }
	*makeHelp { |undocumentedObject, path| ^global.makeHelp(undocumentedObject, path) }
	*makeAutoHelp { |andocumentedClass, path| ^global.makeAutoHelp(andocumentedClass, path) }

		// instance-based getters
	filterUserDirEntries { ^filterUserDirEntries }
	cachePath { ^cachePath }
} // End class


HelpSearchResult {
	var <>docname, <>path, <>goodness, <>context, <>catpath;
	*new{|docname, path, goodness, context, catpath|
		^this.newCopyArgs(docname, path, goodness, context, catpath);
	}

	asString {
		^ "HelpSearchResult(%, %, %, %)".format(docname, path.basename, goodness, this.contextTrimmed)
	}
	// used for sorting:
	<= { |that|
		^ this.goodness >= that.goodness
	}

	contextTrimmed {
		^context.tr($\n, $ ).tr($\t, $ )
	}

	drawRow { |parent, bounds, action|
		// SCButton
		Button.new(parent, bounds.copy.setExtent(bounds.width * 0.3, bounds.height).insetBy(5, 5))
				.states_([[docname]]).action_(action ? { path.openHTMLFile });

		StaticText.new(parent, bounds.copy.setExtent(bounds.width * 0.7, bounds.height)
										.moveBy(bounds.width * 0.3, 0)
										.insetBy(5, 5))
				.string_(this.contextTrimmed);

	}
}


+ Object {

// Classes may override this to specify where they fit in a thematic classification,
// if they want to classify differently than the help doc location would indicate.
//
// Multiple categorisations are allowed (hence the array).
//
// Extension libs (which won't automatically get allocated, since their help won't be in the main
//   help tree) SHOULD override this to specify where best to fit.
//   (Note: *Please* use the "Libraries" and/or "UGens" main classifications, those are the best
//   places for users to find extensions docs. Don't add new "root" help categories, that's
//   not good for GUI usability.)
//
// Each categorisation should be a string using ">" marks to separate categories from subcategories.
// For examples see (e.g.) SinOsc, Gendy1, LPF, Integrator, EnvGen
//*categories {	^ #["Unclassified"]	}
*categories {	^ nil	}

}

+ Pattern {
	*categories {	^ #["Streams-Patterns-Events>Patterns"] }
}

// This allows it to be called from sclang menu item
+ Process {
	helpGui {
		Help.gui
	}
}
