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
	classvar tree, categoriesSkipThese, fileslist;
	classvar <filterUserDirEntries;
	
	*initClass {
		categoriesSkipThese = [Filter, BufInfoUGenBase, InfoUGenBase, MulAdd, BinaryOpUGen, 
						UnaryOpUGen, BasicOpUGen, LagControl, TrigControl, MultiOutUGen, ChaosGen,
			Control, OutputProxy, AbstractOut, AbstractIn, Object, Class];
		filterUserDirEntries = [ "Extensions", "SuperCollider", "SuperCollider3", "Help", "svn", "share", "classes", "trunk", "Downloads" ];
	}
	
	*tree { |sysext=true,userext=true|
		var classes, node, subc, helpRootLen;
		var helpExtensions = ['html', 'scd', 'rtf', 'rtfd'];
		var helpDirs = Array.new;
		var thisHelpExt;
		if(tree.isNil, {
			// Building the tree - base class was originally UGen
			
			// Help file paths - will be used for categorising, if categories is nil or if not a class's helpfile.
			// Otherwise they'll be stored for quick access to helpfile.
			fileslist = IdentityDictionary.new;
			helpDirs = helpDirs.add( Platform.helpDir );
			if ( sysext ,{
				helpDirs = helpDirs.add( Platform.systemExtensionDir );
			});
			if ( userext ,{
				helpDirs = helpDirs.add( Platform.userExtensionDir );
			});
			
			// Now check each class's ".categories" response
			classes = Object.allSubclasses.difference(categoriesSkipThese).reject({|c| c.asString.beginsWith("Meta_")});
			tree = Dictionary.new(8);
			classes.do({|class| this.addCatsToTree(class, fileslist)});
			
			// Now add the remaining ones to the tree - they're everything except the classes which 
	//      have declared their own categorisation(s).
			
			helpDirs.do{ |helpDir|
				this.addDirTree( helpDir,tree );
			};
		});
		^tree;
	}

	*addUserFilter{ |subpath|
		filterUserDirEntries = filterUserDirEntries.add( subpath );
		this.forgetTree;
	}

	*addDirTree{ |helppath,tree|
		var helpExtensions = ['html', 'scd', 'rtf', 'rtfd'];
		var subfileslist;
		var node, subc, helpRootLen, thisHelpExt;

		subfileslist = IdentityDictionary.new;

		PathName.new(helppath.standardizePath).filesDo({|pathname|
				if( helpExtensions.includes(pathname.extension.asSymbol)
					&& pathname.fullPath.contains("3vs2").not
					&& pathname.fullPath.contains("help-scripts").not
					, {
						subfileslist[pathname.fileNameWithoutDoubleExtension.asSymbol] = pathname.fullPath;
						fileslist[pathname.fileNameWithoutDoubleExtension.asSymbol] = pathname.fullPath;
					})
			});

		helpRootLen = (helppath.standardizePath).size + 1;
		subfileslist.keysValuesDo({ |classsym, path|
					
			if ( helppath == Platform.helpDir,
				{
					subc = path[helpRootLen..].split($/);
					subc = subc[0..subc.size-2]; // Ignore "Help" and the filename at the end
				},{
					//helpRootLen = "~".standardizePath;
					if ( helppath == Platform.systemExtensionDir,
						{
							subc = path[helpRootLen..].split($/);
							subc = [ "SystemExtensions" ] ++ subc;
							//subc.postcs;
						});
					if ( helppath == Platform.userExtensionDir,
						{
							helpRootLen = "~/".absolutePath.size + 1;
							subc = path[helpRootLen..].split($/);
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
					subc = subc[..subc.size-2];
				}
			);
			thisHelpExt = helpExtensions.select{ |ext|
				subc.last.endsWith("."++ext)
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
	
	*forgetTree {
		tree = nil;
	}
	
	*dumpTree { |node, prefix=""|
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
	
	*addCatsToTree { |class, fileslist|
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



*gui { |sysext=true,userext=true|
	var classes, win, lists, listviews, numcols=5, selecteditem, node, newlist, curkey, selectednodes, scrollView, compView, textView, buttonView, classButt, browseButt, isClass;
	
	// Call to ensure the tree has been built
	this.tree( sysext, userext );
	
	// Now for a GUI
	win = GUI.window.new("Help browser", Rect(128, 264, 1040, 560)); // SCWindow
	scrollView = GUI.scrollView.new(win, Rect(5, 0, 405, 525)).hasBorder_(true);
	compView = GUI.compositeView.new(scrollView, Rect(0, 0, numcols * 200, 480));
	textView = GUI.textView.new(win, Rect(415, 0, 620, 550))
		.hasVerticalScroller_(true)
		.hasHorizontalScroller_(true)
		.autohidesScrollers_(false)
		.resize_(5)
		.canFocus_(false);
		
	textView.bounds = textView.bounds; // hack to fix origin on first load
	
	lists = Array.newClear(numcols);
	lists[0] = tree.keys(Array).collect(_.asString).sort;
	selectednodes = Array.newClear(numcols);
	
	// SCListView
	listviews = (0..numcols-1).collect({ arg index; var view;
		view = GUI.listView.new( compView, Rect( 5 + (index * 200), 4, 190, 504 ));
		//view.items = []; // trick me into drawing correctly in scrollview
		if( view.respondsTo( \allowsDeselection ), {
			view.allowsDeselection_( true ).value_( nil );
		});
		view
		.resize_(4)
		.action_({ |lv|
			if( lv.value.notNil, {
				// We've clicked on a category or on a class
				if(lv.items.size != 0, { 
					selecteditem = lists[index][lv.value];
					if(listviews[index+1].isNil.not, {
						// Clear the GUI for the subsequent panels
						listviews[index+1..].do({ arg lv; lv.items=#[];
							if( lv.respondsTo( \allowsDeselection ), { lv.value = nil })});
						
						// Get the current node, from the parent node
						node = if(index==0, tree, {selectednodes[index-1]})[selecteditem];
						curkey = selecteditem;
						selectednodes[index] = node;
						
						if(node.isNil, {
							// We have a "leaf" (class or helpdoc), since no keys found
							
							lists[index+1] = #[];

							{textView.open(fileslist.at( selecteditem.asSymbol) ? fileslist.at(\Help))}.defer(0.001);
							isClass = selecteditem.asSymbol.asClass.notNil;							classButt.enabled_(isClass);
							browseButt.enabled_(isClass);
							// The "selectednodes" entry for the leaf, is the path to the helpfile (or "")
							selectednodes[index] = if(index==0, {tree}, {selectednodes[index-1]})
										[curkey.asSymbol.asClass ? curkey.asSymbol];
							
							
						}, {
							// We have a category on our hands
							lists[index+1] = node.keys(Array).collect(_.asString).sort({|a,b| 
								a[0]==$[ || (b[0]!=$[ && (a <= b))
								});
							listviews[index+1].items = lists[index+1];
							
						});
						
						listviews[index+1].value = 1;
						listviews[index+1].valueAction_(0);
						
						selectednodes[index+2 ..] = nil; // Clear out the now-unselected
					});
				});
			});
		});
	});
	
	listviews[0].items = lists[0];
	
	// Add keyboard navigation between columns
	listviews.do({ |lv, index| // SCView
		lv.keyDownAction_({|view,char,modifiers,unicode,keycode|
			var nowFocused;
			nowFocused = lv;
			switch(unicode, 
			63234, { if(index != 0, { listviews[index-1].focus; nowFocused =listviews[index-1] }) }, 
			63235, { if(index != (listviews.size-1) and:{listviews[index+1].items.notNil}, 
							{ try{ listviews[index+1].value_(-1).valueAction_(0).focus; nowFocused =listviews[index+1] } }) },
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
			if(scrollView.visibleOrigin.x > lv.bounds.left, {
				scrollView.visibleOrigin_(Point(lv.bounds.left - 5, 0));
			});	
			if(clickCount == 2, {	
				if(lv.value.notNil and: {if(index==0, tree, {selectednodes[index-1]})[lists[index][lv.value]].isNil}, {
					{ selecteditem.openHelpFile }.defer;
				});
			});
		});
	});
	
	buttonView = GUI.hLayoutView.new(win, Rect(5, 530, 405, 20));
	GUI.button.new(buttonView, Rect(0,0,125, 20))
		.states_([["Open Help File", Color.black, Color.clear]])
		.action_({{ selecteditem.openHelpFile }.defer;});
	classButt = GUI.button.new(buttonView, Rect(0,0,125, 20))
		.states_([["Open Class File", Color.black, Color.clear]])
		.action_({ 
			if(selecteditem.asSymbol.asClass.notNil, {
				{selecteditem.asSymbol.asClass.openCodeFile }.defer;
			});
		});
	browseButt = GUI.button.new(buttonView, Rect(0,0,125, 20))
		.states_([["Browse Class", Color.black, Color.clear]])
		.action_({ 
			if(selecteditem.asSymbol.asClass.notNil, {
				{selecteditem.asSymbol.asClass.browse }.defer;
			});
		});
	
	win.front;
	listviews[0].focus;
	{listviews[0].valueAction_(listviews[0].items.find(["Help"]));}.defer(0.001);
	selecteditem = "Help";
} 
// end *gui

	*all {
		//		^this.new("Help/").dumpToDoc("all-helpfiles");
		var doc;
		var helpExtensions = ['html', 'scd', 'rtf', 'rtfd'];
		var str = CollStream.new;
		doc = Document.new("all-helpfiles");
		[       Platform.helpDir,
			Platform.systemExtensionDir,
			Platform.userExtensionDir
		].do{ |it|
			PathName.new( it ).foldersWithoutSVN.do{ |folderPn|
				str << folderPn.fullPath << Char.nl;
				folderPn.filesDo { |filePn|
					if 
					(helpExtensions.includes(filePn.extension.asSymbol)) {
						str << Char.tab << 
						filePn.fileNameWithoutExtension  << Char.nl;
					}
				};
			}
		};
		doc.string = str.collection;
	}
} // End class





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
