+ Help {
	*addToMenu {
		var ugens, menu;
		var addSubMenu = { |parent, dict, name, index=8|
			var menu = CocoaMenuItem.new(parent, index, name, true);
			var keys = dict.keys.asArray;
			keys.sort {|a,b| a.asString <= b.asString };
			keys.do{ |key, subindex|
				if(dict[key].class == Dictionary) {
					// Add submenu
					addSubMenu.value(menu, dict[key], key[2..key.size-3], subindex)
				}{
					// Add selectable menu item
					CocoaMenuItem.new(menu, subindex, key.asString, false,
						{ key.asString.openHelpFile }
					)
				}
			};
			menu
		};
		addSubMenu.value(CocoaMenuItem.default, Help.tree, "HelpTree");
	}
}
