Plazy : Pattern {
	var <>func;
	*new { arg func;
		^super.new.func_(func)
	}
	//asStream { arg ... args;
//		^func.valueArray(args).asStream
//	}
	embedInStream { arg inval;
		^func.value(inval).embedInStream(inval)
	}
	storeArgs { ^[func] }
}

PlazyEnvir : Plazy {
	var <>passEvent=false;

	embedInStream { arg inval;
		^if(inval.isNil) { func.value.embedInStream(inval) } {
			inval.use {
				if(passEvent) { func.valueEnvir(inval) } { func.valueEnvir }.embedInStream(inval)
			}
		}
	}
}


PlazyEnvirN : PlazyEnvir {
	var genFunc;

	*new { arg func; ^super.new(func).makeGenFunc }

	embedInStream { arg inval;
		var patterns;
		^if(inval.isNil) {
			func.value.embedInStream(inval)
		} {
			inval.use {
				patterns = if(passEvent) { genFunc.valueEnvir(inval) } { genFunc.valueEnvir };
			};
			if(patterns.size > 1) {
				Ppar(patterns).embedInStream(inval)
			} {
				patterns.unbubble.embedInStream(inval)
			}
		}
	}
	// expand arguments
	makeGenFunc {
		genFunc = if(func.def.argNames.notNil) { func.envirFlop } { func }
	}

}

Penvir : Pattern {
	var <>envir, <>pattern, <>independent;
	*new { arg envir, pattern, independent=true;
		^super.newCopyArgs(envir, pattern, independent)
	}
	storeArgs { ^[envir,pattern,independent] }
	embedInStream { arg inval;
		if(independent)
			{ Event.new(8, nil, envir) }
			{ envir }
		.use { pattern.embedInStream(inval) };
		^inval
	}
}


Ppatmod : Pattern {
	var <>pattern, <>func, <>repeats=1;
	*new { arg pattern, func, repeats;
		^super.newCopyArgs(pattern, func, repeats)
	}
	embedInStream { arg inval;
		var localPat;
		localPat = pattern.value(inval).copy;
		repeats.do({ arg i;
			inval = localPat.embedInStream(inval);
			localPat = func.value(localPat, i);
		});
		^inval;
	}
	storeArgs { ^[pattern, func, repeats] }
}


Peventmod : Pattern {
	var <>func, <>event, <>repeats;
	*new { arg func, event, repeats=inf;
		^super.newCopyArgs(func, event, repeats)
	}
	embedInStream { arg inval;
		var localEvent = event.copy ?? { Event.default };
		repeats.value(inval).do { arg i;
			if(inval.isNil) { ^nil.yield };
			localEvent.use { func.valueEnvir(inval, i) };
			inval = localEvent.yield;
		};
		^inval
	}
	storeArgs { ^[func, event, repeats] }
}
