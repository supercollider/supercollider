
Plazy : Pattern {
	var <>func;
	*new { arg func;
		^super.new.func_(func)
	}
	//asStream { arg ... args;
//		^func.valueArray(args).asStream
//	}
	embedInStream { arg inval;				
		^func.value.embedInStream(inval)
	}
	storeArgs { ^[func] }
}

PlazyEnvir : Plazy {
	
	embedInStream { arg inval;
		^if(inval.isNil) { func.value } {
			inval.use {  func.valueEnvir }
		}.embedInStream(inval)
	}
}

Penvir : Pattern {
	var <>envir, <>pattern, <>independent;
	*new { arg envir, pattern, independent=true;
		^super.newCopyArgs(envir, pattern, independent)
	}
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
		^super.new.pattern_(pattern).func_(func).repeats_(repeats)
	}
	embedInStream { arg inval;
		var localPat;
		localPat = pattern.value.copy;
		repeats.do({ arg i;
			inval = localPat.embedInStream(inval);
			localPat = func.value(localPat, i);
		});
		^inval;
	}
	storeArgs { ^[pattern, func, repeats] }
}
