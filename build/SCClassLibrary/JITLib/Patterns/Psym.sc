Psym : FilterPattern {
	var <>dict;
	lookupClass { ^Pdef }
	embedInStream { arg inval;
		var str, outval, pat;
		str = pattern.asStream;
		while {
			outval = str.next(inval);
			outval.notNil
		} {
			inval = (
				(dict ??  { this.lookupClass.all }).at(outval.asSymbol) ?? { Pdef.default }
			).embedInStream(inval);
		}
	
	}
}

Pnsym : Psym {
	lookupClass { ^Pdefn }
}
