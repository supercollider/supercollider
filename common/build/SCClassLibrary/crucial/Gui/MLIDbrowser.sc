
MLIDbrowser { // MultiLevelIdentityDictionary browser

	var <>onSelect,menu,kdr;

	*new { arg ... args;
		var onSelect;
		if(args.size > 1,{
			onSelect = args.pop;
		});
		^super.new
			.onSelect_(onSelect ? { arg item;  item.gui })
			.browse(Library.atList(args) ?? {Library.global.dictionary})
	}

	browse { arg item,parent;
		if(kdr.isNil,{
			kdr = UnicodeResponder.new;
			kdr.registerUnicode(
				KeyCodeResponder.controlModifier | KeyCodeResponder.functionKeyModifier,
				63232 , // cntrl up
				{
					this.browse(parent); // find parent's parent
				});
			kdr.registerUnicode(
				KeyCodeResponder.controlModifier | KeyCodeResponder.functionKeyModifier ,
				63233 , // cntrl down
				{
					// go down on selected
					menu.doAction;
				});
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
			menu.gui(height:min(item.size*30,600));
			menu.keyDownAction = (menu.keyDownResponder ++ kdr);
			menu.focusOn(0);
		})
	}
	// all on one page
	*tree { arg  dictNames, layout ,onSelect;
		^super.new
			.layTree(layout.asPageLayout,
				Library.atList(dictNames) ?? {Library.global.dictionary},
				onSelect ? { arg item;  item.gui },0)
	}
	layTree { arg layout,dict,onSelect,indent=0;
		dict.keys.asList.do({ arg key;
			CXLabel(layout,"",minWidth: indent * 20).background_(Color.clear);
			if(dict.at(key).isKindOf(IdentityDictionary).not,{
				ActionButton(layout,key.asString,{ onSelect.value(dict.at(key)) },minWidth:150);
				layout.startRow;
			},{
				CXLabel(layout,key.asString,minWidth:150);
			layout.startRow;
			this.layTree(layout,dict.at(key),onSelect,indent + 1);
			});
		})
	}
}