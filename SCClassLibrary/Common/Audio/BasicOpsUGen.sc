// These Unit Generators are instantiated by math operations on UGens

BasicOpUGen : UGen {
	var <operator;

	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	nameForDisplay { ^super.name ++ operator }

	operator_ { arg op;
		operator = op;
		specialIndex = operator.specialIndex;
		if(specialIndex < 0) {
			Error("Operator '%' applied to a UGen is not supported in scsynth".format(operator)).throw
		}
	}

	argNamesInputsOffset { ^2 }
	argNameForInputAt { arg i;
		var method = this.class.class.findMethod('new');
		if(method.isNil or: {method.argNames.isNil},{ ^nil });
		^method.argNames.at(i + this.argNamesInputsOffset)
	}
	dumpArgs {
		" ARGS:".postln;
		("   operator:" + operator).postln;
		inputs.do({ arg in,ini;
			("   " ++ (this.argNameForInputAt(ini) ? ini.asString)++":" + in + in.class).postln
		});
	}

	dumpName {
		^synthIndex.asString ++ "_" ++ this.operator
	}
}

UnaryOpUGen : BasicOpUGen {
	*new { arg selector, a;
		^this.multiNew('audio', selector, a)
	}

	init { arg theOperator, theInput;
		this.operator = theOperator;
		rate = theInput.rate;
		inputs = theInput.asArray;
	}

	optimise {
		case
		{ operator == 'neg' } {
			var result = SynthDefOptimisationResult();
			var desc;

			// a - b.neg => a + b
			desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '-' }};
			if (desc.isEmpty.not){
				desc.do{ |minus|
					var others = minus.inputs.select{|i| i != this };
					// b.neg - b.neg is handled inside BinaryOpUgen and can't happen (size would be 0)
					if (others.size == 1){
						var add = BinaryOpUGen.newDuringOptimisation('+', others[0], inputs[0]);
						minus.replaceWith(with: add);
						result.addUGen(add, 2);
						minus.tryDisconnect;
					}
				};
				result.returnNilIfEmpty !? { |r| ^r };
			}
		};
		^nil
	}
}

BinaryOpUGen : BasicOpUGen {
	*new { |selector, a, b| ^this.multiNew('audio', selector, a, b) }

	determineRate { arg a, b;
		if (a.rate == \demand, { ^\demand });
		if (b.rate == \demand, { ^\demand });
		if (a.rate == \audio, { ^\audio });
		if (b.rate == \audio, { ^\audio });
		if (a.rate == \control, { ^\control });
		if (b.rate == \control, { ^\control });
		^\scalar
	}

	*new1 { |rate, selector, a, b|
		// eliminate simple cases
		case
		{ selector == '*' } {
			if (a == 0.0) { ^0.0 };
			if (b == 0.0) { ^0.0 };
			if (a == 1.0) { ^b };
			if (a == -1.0) { ^b.neg };
			if (b == 1.0) { ^a };
			if (b == -1.0) { ^a.neg };
			// TODO: review these
			if (b == 2.0) { ^super.new1(rate, '+', a, a) }; // prefer additions, they are easier to optimise
			if (a == 2.0) { ^super.new1(rate, '+', b, b) };
		}
		{ selector == '+' } {
			if (a == 0.0) { ^b };
			if (b == 0.0) { ^a };
		}
		{ selector == '-' } {
			if (a == 0.0) { ^b.neg };
			if (b == 0.0) { ^a };
			if (a === b) { ^0.0 };  // TODO: review
		}
		{ selector == '/' } {
			if (b == 1.0) { ^a };
			if (b == -1.0) { ^a.neg };
			if (a === b) { ^1.0 }; // TODO: what if it is zero?
		};
		^super.new1(rate, selector, a, b)
	}

	init { arg theOperator, a, b;
		this.operator = theOperator;
		rate = this.determineRate(a, b);
		inputs = [a, b];
	}

	optimizeAddNeg { // TODO: this moves to UnaryOp
		var a, b, replacement;
		#a, b = inputs;
		if(a === b) { ^nil };  // non optimizable edge case.

		if (b.isKindOf(UnaryOpUGen) and: { b.operator == 'neg' and: { b.descendants.size == 1 } }) {
			// a + b.neg -> a - b
			buildSynthDef.removeUGen(b);
			replacement = a - b.inputs[0];
			// this is the first time the dependants logic appears. It's repeated below.
			// We will remove 'this' from the synthdef, and replace it with 'replacement'.
			// 'replacement' should then have all the same descendants as 'this'.
			replacement.descendants = descendants;
			// drop 'this' and 'b' from all of replacement's inputs' descendant lists
			// so that future optimizations decide correctly
			this.optimizeUpdateDescendants(replacement, b);
			^replacement
		};

		if (a.isKindOf(UnaryOpUGen) and: { a.operator == 'neg' and: { a.descendants.size == 1 } }) {
			// a.neg + b -> b - a
			buildSynthDef.removeUGen(a);
			replacement = b - a.inputs[0];
			replacement.descendants = descendants;
			this.optimizeUpdateDescendants(replacement, a);
			^replacement
		};
		^nil
	}


	optimise {
	    // Rules:
	    //   Match on the type of operator.
	    //   Try to apply an optimisation.
	    //   If it works, return a SynthDefOptimisationResult.
	    //   The SynthDefOptimiser will call this method again, so order inside the case blocks matters.

	    case
	    { operator == '*' } {
	        var result = SynthDefOptimisationResult();
	        var addDescendants = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '+' }};
	        if (addDescendants.isEmpty.not){
                addDescendants.do{ |adder|
                    var addValue = adder.inputs.select({|i| i != this })[0];
                    MulAdd.maybeGetMulAddOrder(inputs[0], inputs[1], addValue) !? { |mulAddOrder|
                        var muladd = MulAdd.newDuringOptimisation(*mulAddOrder);
                        adder.replaceWith(with: muladd);
                        result.addUGen(muladd, 2);
                    }
                };
                this.tryDisconnect;
                result.returnNilIfEmpty !? { |r| ^r };
	        }
	    }

	    { operator == '+' } {
            var result = SynthDefOptimisationResult();
            var desc;

            // Get Sum3 descendants and create a Sum4
            desc = descendants.select { |d| d.isKindOf(Sum3) };
            if(desc.isEmpty.not){
                desc.do { |sum3|
                    var otherValues = sum3.inputs.select({|i| i !== this });
                    // sometimes this can be repeated, make sure there are two arguments
                    otherValues = if(otherValues.size == 1) { otherValues[0]!2 } { otherValues };
                    otherValues.debug("otherValues") !? {
                        var sum4 = Sum4.newDuringOptimisation(inputs[0], inputs[1], *otherValues);
                        sum3.replaceWith(with: sum4);
                        result.addUGen(sum4, 2);
                    }
                };
                this.tryDisconnect;
                result.returnNilIfEmpty !? { |r| ^r };
            };

            // Get + descendants and create a Sum3
            desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '+' }};
            if (desc.isEmpty.not){
                desc.do{ |childAdder|
                    childAdder.inputs.select({|i| i != this })[0] !? { |otherValue|
                        var sum3 = Sum3.newDuringOptimisation(inputs[0], inputs[1], otherValue);
                        childAdder.replaceWith(with: sum3);
                        result.addUGen(sum3, 2);
                    };
                };
                this.tryDisconnect;
                result.returnNilIfEmpty !? { |r| ^r };
            }
	    }
	    ^nil
	}


	optimizeSub { // TODO: move this to UnaryOp
		var a, b, replacement;
		#a, b = inputs;

		if (b.isKindOf(UnaryOpUGen) and: { b.operator == 'neg' and: { b.descendants.size == 1 } }) {
			// a - b.neg -> a + b
			buildSynthDef.removeUGen(b);

			replacement = BinaryOpUGen('+', a, b.inputs[0]);
			replacement.descendants = descendants;
			this.optimizeUpdateDescendants(replacement, b);

			synthDef.replaceUGen(this, replacement);
			replacement.optimizeGraph  // not called from optimizeAdd; no need to return ugen here
		};
		^nil
	}
}

MulAdd : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*new { arg in, mul = 1.0, add = 0.0;
		var args = [in, mul, add].asUGenInput(this);
		var rate = args.rate;
		^this.multiNewList([rate] ++ args)
	}
	*new1 { arg rate, in, mul, add;
		var minus, nomul, noadd;

		// eliminate degenerate cases
		if (mul == 0.0, { ^add });
		minus = mul == -1.0;
		nomul = mul == 1.0;
		noadd = add == 0.0;
		if (nomul && noadd, { ^in });
		if (minus && noadd, { ^in.neg });
		if (noadd, { ^in * mul });
		if (minus, { ^add - in });
		if (nomul, { ^in + add });

		if (this.canBeMulAdd(in, mul, add)) {
			^super.new1(rate, in, mul, add)
		};
		if (this.canBeMulAdd(mul, in, add)) {
			^super.new1(rate, mul, in, add)
		};
		^(in * mul) + add
	}
	init { arg in, mul, add;
		inputs = [in, mul, add];
		rate = inputs.rate;
	}

    *maybeGetMulAddOrder { |in, mul, add|
        if(MulAdd.canBeMulAdd(in, mul, add)) { ^[in, mul, add] };
        if(MulAdd.canBeMulAdd(mul, in, add)) { ^[mul, in, add] };
        ^nil
    }
	*canBeMulAdd { arg in, mul, add;
		// see if these inputs satisfy the constraints of a MulAdd ugen.
		if (in.rate == \audio, { ^true });
		if (in.rate == \control
			and: { mul.rate == \control || { mul.rate == \scalar }}
			and: { add.rate == \control || { add.rate == \scalar }},
		{
			^true
		});
		^false
	}
}

Sum3 : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*new { arg in0, in1, in2;
		^this.multiNew(nil, in0, in1, in2)
	}

	*new1 { arg dummyRate, in0, in1, in2;
		var argArray, rate, sortedArgs;
		if (in2 == 0.0) { ^(in0 + in1) };
		if (in1 == 0.0) { ^(in0 + in2) };
		if (in0 == 0.0) { ^(in1 + in2) };

		argArray = [in0, in1, in2];
		rate = argArray.rate;
		sortedArgs = argArray.sort {|a, b| a.rate < b.rate};

		^super.new1(rate, *sortedArgs)
	}
}

Sum4 : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*new { arg in0, in1, in2, in3;
		^this.multiNew(nil, in0, in1, in2, in3)
	}

	*new1 { arg dummyRate, in0, in1, in2, in3;
		var argArray, rate, sortedArgs;

		if (in0 == 0.0) { ^Sum3.new1(nil, in1, in2, in3) };
		if (in1 == 0.0) { ^Sum3.new1(nil, in0, in2, in3) };
		if (in2 == 0.0) { ^Sum3.new1(nil, in0, in1, in3) };
		if (in3 == 0.0) { ^Sum3.new1(nil, in0, in1, in2) };

		argArray = [in0, in1, in2, in3];
		rate = argArray.rate;
		sortedArgs = argArray.sort {|a b| a.rate < b.rate};

		^super.new1(rate, *sortedArgs)
	}
}
