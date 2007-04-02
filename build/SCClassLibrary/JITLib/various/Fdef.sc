
FuncProxy : Ref {
	classvar <>defaultValue=1;
	func { ^value }
	
	value { arg ... args; ^value.valueArray(args) ? defaultValue }
	valueArray { arg args; ^value.valueArray(args) ? defaultValue  }
	valueEnvir { arg ... args; ^value.valueEnvir(*args) ? defaultValue  }
	valueArrayEnvir {  arg ... args; ^value.valueArrayEnvir(args) ? defaultValue  }
	
	source_ { arg obj; if(obj !== this) {  this.value = obj } } // catch at least identity
	source { ^value }
	clear { value = nil }
}


// maybe we want to call "value" "reduce" and only use one class.

Maybe : FuncProxy {
	classvar <callers, <>callFunc;
	classvar <>defaultValue;
	classvar <>protected = false;
	
	value { arg ... args;
		^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { arg args, protect=true;
		^if(protect.not) { 
			value.reduceFuncProxy(args) 
		} {
			this.catchRecursion {
				value.reduceFuncProxy(args)
			}
		} ?? { this.valueEmpty(args) };
	}
	apply { arg args;
		^this.reduceFuncProxy(args, false)
	}
	
	// used by AbstractFunction:reduceFuncProxy 
	// to prevent reduction of enclosed functions
	valueFuncProxy { arg args;
		^this.catchRecursion {
			value.valueFuncProxy(args)
		} ?? { this.valueEmpty(args) };
	}
		
	valueEmpty { arg args;
		(Char.bullet ++ " ? incomplete definition: %\n").postf(this.infoString(args));
		^defaultValue
	}
	
	catchRecursion { arg func;
		var val;
		try {
			protect {
				if(this.includedInCallers) {
					(Char.bullet ++ " ! Couldn't solve a recursive definition in %\n")
						.postf(this.infoString);
					callFunc.value(this, callers, \recursion);
					this.throw;
				};
				callers = callers.add(this);
				val = func.value;
				callFunc.value(this, callers, val);
			} {
				callers.pop;
			};
		}
		^val
	}
	
	includedInCallers {
		^callers.notNil and: { callers.includes(this) }
	}
	
	postString {
		var res = this.findKey;
		^if(res.notNil) { "~" ++ res } { this.asString }
	}
	
	infoString { arg args;
		var who, str="", src;
		who = this.findKey;
		if(who.isNil) { ^this.asString };
		who = "~" ++ who;
		src = this.source.postString;
		if(args.notNil and: {args.notEmpty}) { 
			args = args.collect(_.postString);
			str = "\nArguments passed: %".format(args) 
		};
		^"% <- %".format(who, src.asString) ++ str
	}
	
	findKey {
		^currentEnvironment.findKeyForValue(this)
	}
	
	// use in list comprehension
	all {
		^this.source.all
	}
	do { arg function;
		this.source.do(function) // problem: on the fly change is not picked up.
	}
	
	// math
	composeUnaryOp { arg aSelector;
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { arg aSelector, anArgList;
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}
	
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

/*
MaybeEval : Maybe {
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

MaybeApply : Eval {
	*new { arg key ... args;
		^all[key].valueArray(args)
	}
}
*/




