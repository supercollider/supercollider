
MLIDbrowser { // MultiLevelIdentityDictionary

	var <>onSelect;
	
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
		if(item.isKindOf(IdentityDictionary).not,{
			onSelect.value(item);
		},{
			CXMenu.newWith(
				item.keys.asList.collect({ arg key;
					key->{ this.browse(item.at(key),item) }
				})
			)
			.gui
		})
	}

}