
FuncProxy : Ref {

	func { ^value }
	
	value { arg ... args; ^value.valueArray(args) ? 1 } // default value: 1
	valueArray { arg args; ^value.valueArray(args) ? 1  }
	valueEnvir { arg ... args; ^value.valueEnvir(*args) ? 1  }
	valueArrayEnvir {  arg ... args; ^value.valueArrayEnvir(args) ? 1  }
	
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



