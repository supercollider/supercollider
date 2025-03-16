// maybe we want to call "value" "reduce" and only use one class.

Maybe : Ref {

	classvar <callers, <current, <>callFunc;
	classvar <>defaultValue;
	classvar <>protected = false, <>verbose = false;

	source { ^value }
	source_ { |obj|
		this.value = obj;
		this.changed(\source, obj)
	}

	clear { value = nil }

	value { |... args, kwargs|
		^this.reduceFuncProxy(args, true, kwargs)
	}
	valueArray { |args|
		^this.reduceFuncProxy(args)
	}
	valueEnvir { |... args|
		^this.notYetImplemented(thisMethod)
		//^value.valueEnvir(*args) ? defaultValue
	}
	valueArrayEnvir { |... args|
		^this.notYetImplemented(thisMethod)
		//^value.valueArrayEnvir(args) ? defaultValue
	}
	functionPerformList { |selector, arglist|
		^this.performList(selector, arglist)
	}

	// this allows recursion
	apply { |... args, kwargs|
		^this.reduceFuncProxy(args, false, kwargs)
	}
	// function composition
	o { |... args|
		^NAryValueProxy(this, args)
	}
	<> { |that|
		^o (this, that)
	}

	// override some collection methods

	at { |... args|
		^this.composeNAryOp(\at, args)
	}

	atAll { |... args|
		^this.composeNAryOp(\atAll, args)
	}

	put { |... args|
		^this.composeNAryOp(\put, args)
	}

	putAll { |... args|
		^this.composeNAryOp(\putAll, args)
	}

	add { |... args|
		^this.composeNAryOp(\add, args)
	}

	addAll { |... args|
		^this.composeNAryOp(\addAll, args)
	}

	// use in list comprehension
	all {
		^this.source.all
	}
	do { |function|
		this.source.do(function) // problem: on the fly change is not picked up in this case.
	}

	doesNotUnderstand { |selector ... args, kwargs|
		^this.composeNAryOp(selector, args, kwargs)
	}

	// streams and patterns

	embedInStream { |inval|
		^Prout { |inval|
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
	composeUnaryOp { |aSelector|
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { |aSelector, something, adverb|
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { |aSelector, something, adverb|
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { |aSelector, anArgList, aKeywordArgList|
		^NAryOpFunctionProxy.new(aSelector, this, anArgList, aKeywordArgList)
	}


	// used by AbstractFunction:reduceFuncProxy
	// to prevent reduction of enclosed functions
	valueFuncProxy { |args, kwargs|
		if(verbose and: { value.isNil }) {
			("Maybe: incomplete definition: %\n").postf(this.infoString(args))
		};
		^this.catchRecursion {
			(value ? defaultValue).valueFuncProxy(args, kwargs)
		}
	}

	reduceFuncProxy { |args, protect=true, kwargs|
		if(verbose and: { value.isNil }) {
			("Maybe: incomplete definition: %\n").postf(this.infoString(args))
		};

		^if(protect.not) {
			(value ? defaultValue).reduceFuncProxy(args, protect, kwargs)
		} {
			this.catchRecursion {
				(value ? defaultValue).reduceFuncProxy(args, protect, kwargs)
			}
		}
	}

	catchRecursion { |func|
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

	infoString { |args|
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

	storeOn { |stream|
		// maybe should try to catch a recursion here:
		stream << this.class.name << "(" <<< value << ")" }

}

Fdef : Maybe {
	classvar <>all;

	*initClass {
		all = IdentityDictionary.new
	}

	*new { |key, val|
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
