FuncProxy : Ref {
	
	value { ^value.value }
	valueArray { arg ... args; ^value.valueArray(args) }
	valueEnvir { arg ... args; ^value.valueEnvir(*args) }
	valueArrayEnvir {  arg ... args; ^value.valueArrayEnvir(args) }
}

Fdef : FuncProxy {
	classvar <>all;
	
	*initClass { 
		all = IdentityDictionary.new 
	}
	
	*new { arg key, val;
		var res;
		res = all[key];
		if(res.isNil) {
			res = super.new.value_(val ? 1);
			all[key] = res
		} {
			if(val.notNil) { res.value = val };
		}
		^res
	}

}