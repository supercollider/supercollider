

AbstractConstraint {

	not {	^Not(this)	}
	or { arg constraint;	^Any([this,constraint])	}
	and { arg constraint; ^Every([this,constraint]) }
	xor { arg constraint; ^Xor([this.constraint]) }

	reject { arg constraint;
		^Every([this,Not(constraint)])
	}
	select { arg constraint;
		^Every([this,constraint])
	}
	reset {}

}

Constraint : AbstractConstraint { // seemingly pointless but for .not .reject .select etc.

	var <>function;

	*new { arg function;
		^super.new.function_(function)
	}
	value { arg obj;
		^function.value(obj)
	}
}

Not : AbstractConstraint { // aka TheCompleteOpposite

	var <>constraint;

	*new { arg constraint;
		^super.new.constraint_(constraint)
	}

	value { arg obj;
		^constraint.value(obj).not
	}
	reset { constraint.reset }

}


Every : AbstractConstraint {

	var <>list;

	*new { arg list;
		^super.new.list_(list ?? {[]})
	}
	value { arg obj;
		//list.any({ arg c; c.value(obj).not }).not
		list.do({ arg c;
			if(c.value(obj).not,{ ^false })
		});
		^true
	}
	add { arg constraint;
		list = list.add(constraint)
	}
	reset { list.do({ arg l; l.reset }) }

}

Any : Every {

	value { arg obj;
		list.do({ arg c;
			if(c.value(obj),{ ^true })
		});
		^false
	}

}


Xor : AbstractConstraint { // both are always evaluated

	var <>a,<>b;

	*new { arg a,b;
		^super.new.a_(a).b_(b)
	}

	value { arg obj;
		^a.value(obj) xor: b.value(obj)
	}
	reset { a.reset; b.reset }
}

IsIn : AbstractConstraint { // Includes

	var <>collection;

	*new { arg collection;
		^super.new.collection_(collection)
	}

	value { arg obj;
		^collection.includes(obj)
	}
}

IsNotIn : IsIn { // IncludesNot

	value { arg obj;
		^collection.includes(obj).not
	}

}

SeenBefore : AbstractConstraint {

	var  <>history;
	*new {
		^super.new.reset
	}

	value { arg path;
		if(history.includes(path),{
			^true
		},{
			history=history.add(path);
			^false
		})
	}
	reset {
		history = [];
	}
}

CountLimit : SeenBefore {

	var <>count=0,<>limit;

	*new { arg limit;
		^super.new.limit_(limit)
	}

	value {
		count = count + 1;
		^count <= limit
	}

	reset {
		count = 0;
	}

}

// Unique(stepsToTrack)
// LimitCount(10)
/*

	PreviousValues({arg obj,prev;
			obj.round(0.1) != prev.round(0.1) // none is within 0.1 of a previous value
	});

*/


IsEven : AbstractConstraint {

	value { arg obj; ^obj.even }
	*value { arg obj; ^obj.even }

}
IsOdd : AbstractConstraint {

	value { arg obj; ^obj.odd }
	*value { arg obj; ^obj.odd }

}

IsNil : AbstractConstraint {

	value { arg obj; ^obj.isNil }
	*value { arg obj; ^obj.isNil }

}


NotNil : AbstractConstraint {

	value { arg obj; ^obj.notNil }
	*value { arg obj; ^obj.notNil }

}

