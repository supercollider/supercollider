CocoaMenuItem {
	classvar <topLevelItems;
	classvar default;

	var dataptr;
	var <name;
	var parent; // nil = top level
	var <children;
	var <state = false;
	var <isBranch = false;
	var <>action;

	*clearCustomItems {
		topLevelItems.copy.do({|item| item.remove });
		default = nil;
	}

	*initDefaultMenu {
		default = SCMenuGroup(nil, "Library", 7);
	}

	*default {
		if(default.isNil, {this.initDefaultMenu});
		^default;
	}

	*add { |names, action|
		if(default.isNil) {this.initDefaultMenu };
		^this.deepNew(default, nil, names, action)
	}

	*deepNew { |parent, index, names = #[""], action|
		var menu, name, insertionIndex;
		name = names.removeAt(0);
		insertionIndex = if(names.size <= 1) { index } ? parent.lastIndex;

		menu = parent.findByName(name);
		if(menu.isNil) {
			menu = if(names.size > 0, {SCMenuGroup}, {SCMenuItem})
				.new(parent, name, insertionIndex);
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


SCMenuItem : CocoaMenuItem {

	*new { |parent, name = "", index|
		^super.new.init(parent, index, name);
	}

	init { |argparent, argindex, argname|

		parent = argparent;
		name = argname;
		isBranch = false;
		this.prAddMenuItem(argparent, argindex, argname, isBranch);
		if(parent.notNil && (parent != 'Help')) { parent.addChild(this) }
			{ topLevelItems = topLevelItems.add(this) };
	}
}

SCMenuGroup : CocoaMenuItem {

	*new { |parent, name = "", index|
		^super.new.init(parent, index, name);
	}

	init { |argparent, argindex, argname|

		parent = argparent;
		name = argname;
		isBranch = true;
		this.prAddMenuItem(argparent, argindex, argname, isBranch);
		if(parent.notNil && (parent != 'Help')) { parent.addChild(this) }
			{ topLevelItems = topLevelItems.add(this) };
	}
}

SCMenuRoot {
	*new { ^nil }
}

SCMenuSeparator : CocoaMenuItem {

	*new { arg parent, index;
		^super.new.init( parent, index );
	}

	init { |argparent, argindex|

		parent = argparent;
		isBranch = false;
		this.prAddMenuSeparator(argparent, argindex);
		if(parent.notNil && (parent != 'Help')) { parent.addChild(this) }
			{ topLevelItems = topLevelItems.add(this) };
	}

	prAddMenuSeparator { |parent, index|
		_NewMenuSeparator
	}
}
