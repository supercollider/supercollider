
Plazy : Pattern {
	var <>func;
	*new { arg func;
		^super.new.func_(func)
	}
	asStream { arg ... args;
		^func.valueArrayEnvir(args).asStream
	}
}


Ppatmod : Pattern {
	var <>pattern, <>func, <>repeats=1;
	*new { arg pattern, func, repeats;
		^super.new.pattern_(pattern).func_(func).repeats_(repeats)
	}
	asStream { 
		^Routine.new({ arg inval;
			var localPat;
			localPat = pattern.value.copy;
			repeats.do({ arg i;
				inval = localPat.embedInStream(inval);
				localPat = func.value(localPat, i);
			});
		})
	}
}

