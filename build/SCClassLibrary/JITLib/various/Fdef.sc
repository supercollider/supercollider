
FuncProxy : Ref {

	func { ^value }
	
	value { arg ... args; ^value.valueArray(args) ? this } 
	valueArray { arg args; ^value.valueArray(args) ? this  }
	valueEnvir { arg ... args; ^value.valueEnvir(*args) ? this  }
	valueArrayEnvir {  arg ... args; ^value.valueArrayEnvir(args) ? this  }
	
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
			res = super.new.source_(val ? 1);
			all[key] = res
		} {
			if(val.notNil) { res.source = val };
		}
		^res
	}

}



