
EnvEditor : Editor {
	// could make the curve an array automatically

	// for playing, send the array editable on server
	// but saves as Env which is fixed

	var <>levelSpec;

	*new { arg env,levelSpec;
		if(levelSpec.isNil,{
			levelSpec = StaticSpec(env.levels.minItem,env.levels.maxItem);
		});
		^super.new.value_(env).levelSpec_(levelSpec.asSpec)
	}
	instrArgFromControl { arg control;
		^value
	}
	value { // i return a copy for safety
		^value.deepCopy
	}
	// you must access everything through me;
	putTime { arg i,t;
		value.times.put(i,t);
		value.changed;
		this.changed;
	}
	putLevel { arg i,l;
		value.levels.put(i,l);
		value.changed;
		this.changed;
	}
	setCurve {arg c;
		if(c.isFloat,{
			// curvatures may not be within 0.001 of 0.0
			// or EnvGen blows up ?!
			if(c.abs < 0.001,{ c = 0.001 });// sign not significant at this magnitude
			value.curves = c;
		},{
			value.curves = c;
		});
		value.changed;
		this.changed;
	}
	putCurve { arg i,c;
		// curvatures may not be within 0.001 of 0.0
		// or EnvGen blows up ?!
		if(c.abs < 0.001,{ c = 0.001 });
		value.curves.put(i,c);
		value.changed;
		this.changed;
	}
	times { ^value.times }
	levels { ^value.levels }
	curves { ^value.curves }
	spec { ^EnvSpec(this.value) }
	//pr
	env {// direct access for the gui
		^value // the prototype env that we edit
	}
	copy { ^this.class.new(value.deepCopy,levelSpec) }
	guiClass { ^EnvEditorGui }

}

