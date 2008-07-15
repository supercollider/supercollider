CocoaMenuItem {
	classvar topLevelItems;
	
	var dataptr;
	var name;
	var parent; // nil = top level
	var <>action;
	var children;
	var <state = false;
	
	*new { |parent, index, name = "", submenu = false, action|
		^super.new.init(parent, index, name, submenu, action);
	}
	
	*clearCustomItems {
		topLevelItems.do({|item| item.remove });
	}
	
	init { |argparent, argindex, argname, submenu, argaction|
		
		parent = argparent;
		name = argname;
		action = argaction;
		parent.notNil.if({parent.addChild(this)}, {topLevelItems = topLevelItems.add(this);});
		this.prAddMenuItem(argparent, argindex, argname, submenu);
	}
	
	remove {
		children.do({|child| child.remove}); // cleanup my kids
		children = nil;
		this.prRemoveMenuItem;
	}
	
	addChild {|child|
		children = children.add(child);
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
	
	prAddMenuItem { |parent, index, name, submenu|
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