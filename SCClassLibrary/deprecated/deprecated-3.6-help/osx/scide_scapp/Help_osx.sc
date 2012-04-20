+ Help {
	*addToMenu {
		var ugens, menu;
		var addSubMenu = { |parent, dict, name, index=8|
			var menu = SCMenuGroup.new(parent, name, index);
			var keys = dict.keys.asArray;
			keys.sort {|a,b| a.asString <= b.asString };
			keys.do{ |key, subindex|
				if(dict[key].class == Dictionary) {
					// Add submenu
					addSubMenu.value(menu, dict[key], key[2..key.size-3], subindex)
				}{
					// Add selectable menu item
					SCMenuItem.new(menu, key.asString, subindex).action_(
						{ key.asString.help }
					)
				}
			};
			menu
		};
		addSubMenu.value('Help', Help.tree, "Help Tree");
	}
}
