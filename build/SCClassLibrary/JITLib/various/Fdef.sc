
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
	classvar <callers, <current, <>callFunc;
	classvar <>defaultValue;
	classvar <>protected = false, <>verbose = false;
	
	
	value { arg ... args;
		^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { arg args, protect=true;
		//this.postln;
		^if(protect.not) { 
			value.reduceFuncProxy(args) 
		} {
			this.catchRecursion {
				value.reduceFuncProxy(args)
			}
		} ?? { this.valueEmpty(args) };
	}
	// this allows recursion
	apply { arg ... args;
		^this.reduceFuncProxy(args, false)
	}
	// function composition
	o { arg ... args;
		^NAryValueProxy(this, args)
	}
	<> { arg that;
		^o (this, that)
	}
	
	
	// used by AbstractFunction:reduceFuncProxy 
	// to prevent reduction of enclosed functions
	valueFuncProxy { arg args;
		^this.catchRecursion {
			value.valueFuncProxy(args)
		} ?? { this.valueEmpty(args) };
	}
		
	valueEmpty { arg args;
		if(verbose) {
			(Char.bullet ++ " ? incomplete definition: %\n").postf(this.infoString(args))
		};
		^defaultValue
	}
	
	catchRecursion { arg func;
		var val, previous;
		try {
			protect {
				previous = current;
				current = this;
				
				if(this.includedInCallers) {
					if(verbose) {
						(Char.bullet ++ " ! Couldn't solve a recursive definition in %\n")
						.postf(this.infoString)
					};
					callFunc.value(this, callers, \recursion);
					this.throw;
				};
				callers = callers.add(this);
				val = func.value;
				callFunc.value(this, callers, val);
			} {
				callers.pop;
				current = previous;
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
	
	doesNotUnderstand { arg selector ... args;
		^this.composeNAryOp(selector, args)
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
	storeOn { arg stream; stream << this.class.name << "(" <<< value << ")" }
	
}

// dependency scheme

Perhaps : Maybe {

	var <>connected, <>action;
		
	// find better name.
	glue { 
		current !? {
			this.addDependant(current);
			current.addConnection(this);
		}
	}
	
	releaseGlued {
		connected !? { 
			connected.do { |x|
				x.removeDependant(this);
			};
			connected.makeEmpty;
		}
	}
	
	addConnection { arg obj;
		if(connected.isNil) { connected = IdentitySet.new };
		connected.add(obj);
	}
	
	value_ { arg obj;
		this.releaseGlued;
		value = obj;
		this.update(nil, obj); // recursion?
		this.changed(obj);
	}
	
	update { arg who, what;
		if(who !== this) {
			"updated % %\n".postf(who, what);
			this.doAction(who, what)
		};	
	}
	
	doAction { arg who, what;
		this.action.value(this, what); 
		// we may need to catch circularity somewhere here.
		// maybe with a catchRecursion.
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
