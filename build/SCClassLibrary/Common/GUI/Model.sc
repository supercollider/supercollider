Model {
	var <>dependants;
	
	changed { arg what ... moreArgs;
		dependants.do({ arg item;
			item.update(this, what, *moreArgs);
		});
	}
	addDependant { arg dependant;
		if (dependants.isNil, {
			dependants = IdentitySet.new(4);
		});
		dependants.add(dependant);
	}
	removeDependant { arg dependant;
		if (dependants.notNil, {
			dependants.remove(dependant);
		});
	}
	release {
		dependants = nil;
	}
}

SimpleController {
	var model, actions;
	// responds to updates of a model
	
	*new { arg model;
		^super.newCopyArgs(model).init
	}
	init {
		model.addDependant(this);
	}
	put { arg what, action;
		if (actions.isNil, {
			actions = IdentityDictionary.new(4);
		});
		actions.put(what, action);
	}
	update { arg theChanger, what ... moreArgs;
		var action;
		action = actions.at(what);
		if (action.notNil, {
			action.valueArray(theChanger, what, moreArgs);
		});
	}
	remove {
		model.removeDependant(this);
	}
}

TestDependant {
	update { arg thing;
		(thing.asString ++ " was changed.\n").post;
	}
}