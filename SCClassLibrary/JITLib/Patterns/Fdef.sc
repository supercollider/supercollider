// maybe we want to call "value" "reduce" and only use one class.

Maybe : Ref {

	classvar <callers, <current, <>callFunc;
	classvar <>defaultValue;
	classvar <>protected = false, <>verbose = false;
	classvar <>defaultValue=1;

	source { ^value }
	source_ { arg obj;
		this.value = obj;
		this.changed(\source, obj)
	}

	clear { value = nil }

	value { arg ... args;
		^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}
	valueEnvir { arg ... args;
		^this.notYetImplemented(thisMethod)
		//^value.valueEnvir(*args) ? defaultValue
	}
	valueArrayEnvir {  arg ... args;
		^this.notYetImplemented(thisMethod)
		//^value.valueArrayEnvir(args) ? defaultValue
	}
	functionPerformList { arg selector, arglist;
		^this.performList(selector, arglist)
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

	// use in list comprehension
	all {
		^this.source.all
	}
	do { arg function;
		this.source.do(function) // problem: on the fly change is not picked up in this case.
	}

	doesNotUnderstand { arg selector ... args;
		^this.composeNAryOp(selector, args)
	}

	// streams and patterns

	embedInStream { arg inval;
		^Prout { arg inval;
			var curVal, str;
			var outval;
			while {
				if(curVal !== value) { str = value.asStream; curVal = value };
				outval = str.next(inval);
				outval.notNil
			} {
				inval = outval.yield;
			}
		}.embedInStream(inval)
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


	// used by AbstractFunction:reduceFuncProxy
	// to prevent reduction of enclosed functions
	valueFuncProxy { arg args;
		^this.catchRecursion {
			value.valueFuncProxy(args)
		} ?? { this.valueEmpty(args) };
	}

	valueEmpty { arg args;
		if(verbose) {
			("* ? incomplete definition: %\n").postf(this.infoString(args))
		};
		^defaultValue
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

	catchRecursion { arg func;
		var val, previous;
		try {
			protect {
				previous = current;
				current = this;

				if(this.includedInCallers) {
					if(verbose) {
						("* ! Couldn't solve a recursive definition in %\n")
						.postf(this.infoString)
					};
					callFunc.value(this, callers, \recursion);
					this.throw;
				};
				// add this to the list of current callers
				callers = callers.add(this);
				// evaluate function
				val = func.value;

				callFunc.value(this, callers, val);

			} { |exception|
				if(verbose and: { exception.isKindOf(Exception)} ) {
					("Error or incomplete specification" + exception.errorString).postln;
				};
			/*	if(exception.isKindOf(this.class).not) {
					Exception.throw;
				}*/
				// remove again
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

	storeOn { arg stream;
	// maybe should try to catch a recursion here:
	stream << this.class.name << "(" <<< value << ")" }

}

Fdef : Maybe {
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
