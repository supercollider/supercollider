
/*

UserPanel {
	classvar <allPanels;
	var filename, <>actions;
	
	*new { arg filename, actions;
		^super.newCopyArgs(filename, actions).init;
	}
	init { 
		allPanels = allPanels.add(this);
		this.prInit(filename);
	}
	doAction { arg tag, float, string;
		[tag, float, string].postln;
//		var action;
//		action = actions.at(tag);
//		if (action.notNil, {
//			action.value(tag);
//		})
	}
	closed {
		\closed.postln;
		allPanels.remove(this);
	}
	
	
	// PRIVATE;
	prInit { arg filename;
		_LoadUserPanel
		^this.primitiveFailed
	}
}

*/
