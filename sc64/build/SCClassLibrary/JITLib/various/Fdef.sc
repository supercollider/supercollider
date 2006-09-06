
FuncProxy : Ref {
	classvar <>defaultValue=1;
	func { ^value }
	
	value { arg ... args; ^value.valueArray(args) ? defaultValue }
	valueArray { arg args; ^value.valueArray(args) ? defaultValue  }
	valueEnvir { arg ... args; ^value.valueEnvir(*args) ? defaultValue  }
	valueArrayEnvir {  arg ... args; ^value.valueArrayEnvir(args) ? defaultValue  }
	
	source_ { arg obj; if(obj !== this) {  this.value = obj } } // catch at least identity
	clear { value = nil }	
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
			res = super.new.source_(val);
			all[key] = res
		} {
			if(val.notNil) { res.source = val };
		}
		^res
	}

}



