
MLIDbrowser { // MultiLevelIdentityDictionary

	var <>onSelect,menu,kdr;
	
	*new { arg ... args;
		var onSelect;
		if(args.size > 1,{
			onSelect = args.pop;
		});
		^super.new
			.onSelect_(onSelect ? { arg item;  item.topGui })
			.browse(Library.atList(args) ?? {Library.global.dictionary})
	}
	
	browse { arg item,parent;
		if(kdr.isNil,{
			kdr = KeyCodeResponder.new;
			kdr.registerKeycode(262144,126,{ //cntl up
				// go up
				this.browse(parent); // find parent's parent
			});
			kdr.registerKeycode(262144,125,{ // cntl down
				// go down on selected
				menu.doAction;
			});
			kdr.registerKeycode(0,53,{
				this.browse(parent);
			})
		});
		if(item.isKindOf(IdentityDictionary).not,{
			onSelect.value(item);
		},{
			menu = nil;
			if(menu.isNil,{ menu = CXMenu.new });
			menu.nameFuncs = 
				item.keys.asList.collect({ arg key;
					key->{ this.browse(item.at(key),item) }
				});
			menu.gui;
			menu.keyDownAction = (menu.keyDownResponder ++ kdr);
			menu.focusOn(0);
		})
	}

}