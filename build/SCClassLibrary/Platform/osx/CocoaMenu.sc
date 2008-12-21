CocoaMenuItem {
	classvar <topLevelItems;
	classvar <default;

	var dataptr;
	var <name;
	var parent; // nil = top level
	var <children;
	var <state = false;
	var <isBranch = false;
	var <>action;
	
	*new { |parent, index, name = "", hasSubmenu = false, action|
		^super.new.init(parent, index, name, hasSubmenu, action);
	}
	
	*clearCustomItems {
		topLevelItems.copy.do({|item| item.remove });
	}
	
	*initDefaultMenu {
		default = CocoaMenuItem(nil, 9, "Library", true);
	}
	
	*add { |names, action|
		if(default.isNil) {this.initDefaultMenu };
		^this.deepNew(default, default.lastIndex, names, action)
	}
	
	*deepNew { |parent, index, names = #[""], action|
		var menu, name;
		name = names.removeAt(0);
		index = index ?? { parent.lastIndex };
		
		menu = parent.findByName(name);
		if(menu.isNil) {
			menu = this.new(parent, index, name, names.size > 0) 
		};
		
		if(names.size < 1) {
			if(menu.isBranch) {
				"failed to add menu item: item % has submenu".format(name.quote).warn;
				menu = nil;
			} {
				menu.action = action;
			}
		} {
			if(menu.isBranch.not) {
				"failed to add menu item: item % allows no submenu".format(name.quote).warn;
				menu = nil;
			} {
				menu = this.deepNew(menu, index, names, action)
			}
		};
		^menu
	}
	
	lastIndex {
		^children.asArray.size
	}
	
	findByName { |name|
		^children.detect { |x| x.name == name };
	}
	
	init { |argparent, argindex, argname, hasSubmenu, argaction|
		
		parent = argparent;
		name = argname;
		action = argaction;
		isBranch = hasSubmenu;
		this.prAddMenuItem(argparent, argindex, argname, hasSubmenu);
		if(parent.notNil && (parent != 'Help')) { parent.addChild(this) } 
			{ topLevelItems = topLevelItems.add(this) };
	}
	
	remove {
		children.copy.do({|child| child.remove}); // cleanup my kids
		children = nil;
		(parent.notNil && (parent != 'Help')).if({parent.removeChild(this)});
		this.prRemoveMenuItem;
		topLevelItems.remove(this);
	}
	
	addChild {|child|
		children = children.add(child);
	}
	
	removeChild {|child|
		children.remove(child);
	}
	
	enabled_ {|bool|
		_EnableMenuItem
		^this.primitiveFailed;
	}
	
	state_ {|bool|
		state = bool;
		this.prSetState(bool);
	}
	
	// Cmd is assumed
	setShortCut {|string, alt = false, ctrl = false|
		_SetMenuItemKeyboardEquivalent
	}
	
	prSetState {|bool|
		_SetMenuItemState
		^this.primitiveFailed;
	}
	
	prAddMenuItem { |parent, index, name, hasSubmenu|
		_NewMenuItem
		^this.primitiveFailed;
	}
	
	prRemoveMenuItem {
		_RemoveMenuItem
		^this.primitiveFailed;
	}
	
	doAction {
		action.value(this);
	}
	
	
}