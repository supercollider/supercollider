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
		if(actions.notNil) {
			action = actions.at(what);
			if (action.notNil, {
				action.valueArray(theChanger, what, moreArgs);
			});
		};
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




NotificationCenter {

	classvar registrations;

	//			who		\didSomething
	*notify { arg object, message, args;
		registrations.at(object,message).copy.do({ arg function;
			function.valueArray(args)
		})
	}

	// 			who		\didSomething
	*register { arg object,message,listener,action;
		var nr;
		nr = NotificationRegistration(object,message,listener);
		registrations.put(object,message,listener,action);
		^nr;
	}

	*unregister { arg object,message,listener;
		var lastKey,lastDict;
		lastDict = registrations.at(object,message);
		if(lastDict.notNil,{
			lastDict.removeAt(listener);
			(lastDict.size == 0).if({
				registrations.removeAt(object,message);
				(registrations.at(object).size == 0).if({
					registrations.removeAt(object);
				});
			});
		});
	}
	*registerOneShot {  arg object,message,listener,action;
		var nr;
		nr = NotificationRegistration(object,message,listener);
		registrations.put(object,message,listener,
			{ |args|
				action.value(args);
				this.unregister(object,message,listener)
			});
		^nr
	}
	*clear {
		registrations = MultiLevelIdentityDictionary.new;
	}
	*registrationExists { |object,message,listener|
		^registrations.at(object,message,listener).notNil
	}
	*initClass {
		this.clear
	}
	*removeForListener { |listener|
		registrations.removeAt(listener)
	}
}

NotificationRegistration {

	var <>object,<>message,<>listener;

	*new { arg o,m,l;
		^super.new.object_(o).message_(m).listener_(l)
	}
	remove {
		NotificationCenter.unregister(object,message,listener)
	}
}
