Model {
	var <>dependants;
	
	changed { arg what ... moreArgs;
		if (dependants.notNil, {
			dependants.do({ arg item;
				item.performList(\update, this, what, moreArgs);
			});
		});
	}
	addDependant { arg dependant;
		this.checkDependants;
		dependants.add(dependant);
	}
	removeDependant { arg dependant;
		if (dependants.notNil, {
			dependants.remove(dependant);
		})
	}
	release {
		dependants = nil;
	}
	checkDependants {
		if (dependants.isNil, {
			dependants = IdentitySet.new(4);
		})
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