UnaryOpPlug : AbstractFunction {
	var >operator, >a;
	*new { arg operator, a;
		^super.newCopyArgs(operator, a)
	}
	value { arg proxy;
		^a.value(proxy).perform(operator)
	}
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
}


BinaryOpPlug : AbstractFunction  {
	var >operator, >a, >b;
	*new { arg operator, a, b;	
		^super.newCopyArgs(operator, a, b)
	}
	
	value { arg proxy;
		var vala, valb;
		vala = a.value(proxy);
		if(proxy.isNil or: { proxy.isNeutral }, { proxy = a });
		valb = b.value(proxy);
		^vala.perform(operator, valb)
	}
	
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
	
}

