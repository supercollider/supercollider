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
	
	*initClass {
		categoriesSkipThese = [Filter, BufInfoUGenBase, InfoUGenBase, MulAdd, BinaryOpUGen, 
						UnaryOpUGen, BasicOpUGen, LagControl, TrigControl, MultiOutUGen, ChaosGen,
						Control, OutputProxy, AbstractOut, AbstractIn, Object, Class];	}
	
	*tree {
		var classes, node, subc, helpRootLen;
		if(tree.isNil, {
			// Building the tree - base class was originally UGen
			
			// Help file paths - will be used for categorising, if categories is nil or if not a class's helpfile.
			// Otherwise they'll be stored for quick access to helpfile.
			fileslist = IdentityDictionary.new;
			PathName.new(Platform.helpDir).filesDo({|pathname|
				if(pathname.fileName.endsWith(".html") 
						&& pathname.fullPath.contains("3vs2").not
						&& pathname.fullPath.contains("help-scripts").not
						, {
					fileslist[pathname.fileNameWithoutExtension.asSymbol] = pathname.fullPath;
				})
			});
			
			// Now check each class's ".categories" response
			classes = Object.allSubclasses.difference(categoriesSkipThese).reject({|c| c.asString.beginsWith("Meta_")});
			tree = Dictionary.new(8);
			classes.do({|class| this.addCatsToTree(class, fileslist)});
			
			// Now add the remaining ones to the tree - they're everything except the classes which 
			//      have declared their own categorisation(s).
			helpRootLen = (Platform.helpDir).size + 1;
			fileslist.keysValuesDo({ |classsym, path|
				
				subc = path[helpRootLen..].split($/);
				subc = subc[0..subc.size-2]; // Ignore "Help" and the filename at the end
				if(subc.last.endsWith(".html"), {
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
				node[classsym.asClass ? classsym] = path;			});
		});
		^tree;
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



*gui {
	var classes, win, lists, listviews, numcols=5, selecteditem, node, newlist, curkey, selectednodes;
	
	// Call to ensure the tree has been built
	this.tree;
	
	// Now for a GUI
	win = GUI.window.new("Help browser", Rect(128, 264, 200*numcols, 500)); // SCWindow
	
	lists = Array.newClear(numcols);
	lists[0] = tree.keys(Array).collect(_.asString).sort;
	selectednodes = Array.newClear(numcols);
	
	// SCListView
	listviews = (0..numcols-1).collect({|index|
		GUI.listView.new(win, Rect(10 + (index * 200), 5, 180, 480)).action_({ |lv|
			var selectedstr = lv.items[lv.value];		

			if(selectedstr[0..1]!="->", {
				// We've clicked on a category or on a class
				if(lv.items.size != 0, { 
					selecteditem = lists[index][lv.value];
					if(listviews[index+1].isNil.not, {
						// Clear the GUI for the subsequent panels
						listviews[index+1..].do(_.items=#[]);
						
						// Get the current node, from the parent node
						node = if(index==0, tree, {selectednodes[index-1]})[selecteditem];
						curkey = selecteditem;
						selectednodes[index] = node;
						
						if(node.isNil, {
							// We have a "leaf" (class or helpdoc), since no keys found
							
							lists[index+1] = #[];
							listviews[index+1].items = 
								#["->Help"] ++ if(curkey.asSymbol.asClass.isNil, nil, #["->Source"]);
									
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
			switch(unicode, 
			63234, { if(index != 0, { listviews[index-1].focus }) }, 
			63235, { if(index != (listviews.size-1) and:{listviews[index+1].items.notNil}, 
							{ try{ listviews[index+1].value_(-1).valueAction_(0).focus } }) },
			13, { // Hit RETURN to open source or helpfile
				// The class name, or helpfile name we're after
				node = selecteditem;
				switch(lv.items[lv.value][2..], 
					"Help",   { { node.openHelpFile }.defer; }, 
					"Source", { { node.asSymbol.asClass.openCodeFile }.defer; }
				);
			},
			//default:
			{ 
				// Returning nil is supposed to be sufficient to trigger the default action,
				// but on my SC this doesn't happen.
				view.defaultKeyDownAction(char,modifiers,unicode);
			});
		})
		.mouseDownAction_({
				// The class name, or helpfile name we're after
				node = selecteditem;
				switch(lv.items[lv.value][2..], 
					"Help",   { { node.openHelpFile }.defer; }, 
					"Source", { { node.asSymbol.asClass.openCodeFile }.defer; }
				);
		});
	});
	
	win.front;
	listviews[0].focus;
} // end *gui

	*all {
		//		^this.new("Help/").dumpToDoc("all-helpfiles");
		var doc;
		var str = CollStream.new;
		doc = Document.new("all-helpfiles");
		[ 
			Platform.classLibraryDir,
			Platform.systemAppSupportDir,
			Platform.userAppSupportDir,
			Platform.systemExtensionDir,
			Platform.userExtensionDir
		].do{ |it| PathName.new( it ).foldersWithoutSVN.select{ |it| it.fullPath.contains("Help") }.do{ |help| help.streamTree(str) } };
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
