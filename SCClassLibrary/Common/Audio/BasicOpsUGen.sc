// These Unit Generators are instantiated by math operations on UGens

BasicOpUGen : UGen {
	var <operator;

	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }
	nameForDisplay { ^(super.name ++ operator).asSymbol }

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
		if (inputs[0].source.isKindOf(DC) and: { inputs[0].source.rate == this.rate }) {
			var result = SynthDefOptimisationResult();
			this.replaceInputAt(0, inputs[0].source.inputs[0]);
			result.addUGen(this, 1);
			^result;
		};

		if (inputs[0].isKindOf(Number)) {
			var result = SynthDefOptimisationResult();
			var answer = inputs[0].perform(this.operator);
			result.addConstantsDescendants(this.descendants);
			this.replaceWith(answer);
			^result;
		};

		case
		{ operator == 'neg' } {
			var result = SynthDefOptimisationResult();
			var desc;

			// a - b.neg => a + b
			desc = descendants.select { |d|
				d.isKindOf(BinaryOpUGen) and: { d.operator == '-' }
			};
			if (desc.isEmpty.not){
				desc.do{ |minus|
					var other = minus.inputs[0];
					if (other != this) {
						var add = BinaryOpUGen.newDuringOptimisation(this.rate, '+', other, inputs[0]);
						minus.replaceWith(with: add);
						result.addUGen(add, 2);
					}
				};
				result.returnNilIfEmpty !? { |r| ^r };
			};


			desc = descendants.select { |d|
				d.isKindOf(BinaryOpUGen) and: { d.operator == '+' }
			};
			if (desc.isEmpty.not){
				desc.do{ |add|
					var other = add.inputs[0];
					if (other != this) {
						var minus = BinaryOpUGen.newDuringOptimisation(this.rate, '-', other, inputs[0]);
						add.replaceWith(with: minus);
						result.addUGen(minus, 2);
						add.tryDisconnect;
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
		^super.new1(rate, selector, a, b)
	}

	init { arg theOperator, a, b;
		this.operator = theOperator;
		rate = this.determineRate(a, b);
		inputs = [a, b];
	}

	optimise {
		// Rules:
		//   Match on the type of operator.
		//   Try to apply an optimisation.
		//   If it works, return a SynthDefOptimisationResult.
		//   The SynthDefOptimiser will call this method again, so order inside the case blocks matters.

		// Remove all DC inputs in favour of constants as they can be optimised further.
		if (inputs.any({ |in| in.source.isKindOf(DC) })) {
			var result = SynthDefOptimisationResult();
			inputs.size.do { |i|
				var in = inputs[i];
				if (in.source.isKindOf(DC) and: { in.source.rate == this.rate }) {
					this.replaceInputAt(i, in.inputs[0]);
					result.addUGen(this, 1);
				}
			};
			^result;
		};

		if (inputs[0].isKindOf(Number) and: { inputs[1].isKindOf(Number) }) {
			var result = SynthDefOptimisationResult();
			var answer = inputs[0].perform(this.operator, inputs[1]);
			result.addConstantsDescendants(this.descendants);

			this.replaceWith(answer);
			^result;
		};

		case
		{ operator == '*' } {
			var result = SynthDefOptimisationResult();
			var desc;

			// a * 1 => a
			if (inputs[0] == 1){
				result.addUGen(inputs[1], 1);
				this.replaceWith(inputs[1]);
				^result;
			};
			// 1 * a => a
			if (inputs[1] == 1){
				result.addUGen(inputs[0], 1);
				this.replaceWith(inputs[0]);
				^result;
			};

			// (a * 0) | (0 * a) => 0
			if (inputs[0] == 0 or: { inputs[1] == 0 }){
				result.addConstantsDescendants(this.descendants);
				this.replaceWith(0);
				^result;
			};

			// a * -1 => a.neg
			if (inputs[0] == -1){
				var new = UnaryOpUGen.newDuringOptimisation(this.rate, 'neg', inputs[1]);
				this.replaceWith(new);
				result.addUGen(new, 1);
				^result;
			};
			// -1 * a => a.neg
			if (inputs[1] == -1){
				var new = UnaryOpUGen.newDuringOptimisation(this.rate, 'neg', inputs[0]);
				this.replaceWith(new);
				result.addUGen(new, 1);
				^result;
			};

			// a + b * c => MulAdd(a, b, c);
			desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '+' }};
			if (desc.isEmpty.not){
				desc.do{ |adder|
					var addValue = adder.inputs.select({|i| i != this })[0];
					MulAdd.maybeGetMulAddOrder(inputs[0], inputs[1], addValue) !? { |mulAddOrder|
						var muladd = MulAdd.newDuringOptimisation(this.rate, *mulAddOrder);
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

			// 0 + a => a
			if (inputs[0] == 0){
				result.addUGen(inputs[1], 1);
				this.replaceWith(inputs[1]);
				^result;
			};

			// 0 + a => a
			if (inputs[1] == 0){
				result.addUGen(inputs[0], 1);
				this.replaceWith(inputs[0]);
				^result;
			};

			// Get Sum3 descendants and create a Sum4
			desc = descendants.select { |d| d.isKindOf(Sum3) };
			if(desc.isEmpty.not){
				desc.do { |sum3|
					var otherValues = sum3.inputs.select({|i| i !== this });
					// sometimes this can be repeated, make sure there are two arguments
					otherValues = if(otherValues.size == 1) { otherValues[0]!2 } { otherValues };
					otherValues !? {
						var sum4 = Sum4.newDuringOptimisation(this.rate, inputs[0], inputs[1], *otherValues);
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
						var sum3 = Sum3.newDuringOptimisation(this.rate, inputs[0], inputs[1], otherValue);
						childAdder.replaceWith(with: sum3);
						result.addUGen(sum3, 2);
					};
				};
				this.tryDisconnect;
				result.returnNilIfEmpty !? { |r| ^r };
			}
		}

		{ operator == '-' } {
			var result = SynthDefOptimisationResult();
			var desc;

			// 0 - a => a
			if (inputs[0] == 0){
				result.addUGen(inputs[1], 1);
				this.replaceWith(inputs[1]);
				^result;
			};
			// 0 - a => a
			if (inputs[1] == 0){
				result.addUGen(inputs[0], 1);
				this.replaceWith(inputs[0]);
				^result;
			}
		};
		^nil
	}
}

MulAdd : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg in, mul = 1.0, add = 0.0;
		var args = [in, mul, add].asUGenInput(this);
		var rate = args.rate;
		^this.multiNewList([rate] ++ args)
	}
	*new1 { arg rate, in, mul, add;
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
		if (in.rate == \audio) { ^true };
		if (in.rate == \control
			and: { mul.rate == \control || { mul.rate == \scalar }}
			and: { add.rate == \control || { add.rate == \scalar }})
		{ ^true };

		^false
	}

	optimise {
		// 0 * a + b => b
		if (inputs[0] == 0){
			var result = SynthDefOptimisationResult();
			this.replaceWith(inputs[2]);
			result.addUGen(inputs[2], 1);
			^result;
		};
		// a * 0 + b => b
		if (inputs[1] == 0){
			var result = SynthDefOptimisationResult();
			this.replaceWith(inputs[2]);
			result.addUGen(inputs[2], 1);
			^result;
		};


		// -1 * a + b => b - a
		if (inputs[0] == -1){
			var result = SynthDefOptimisationResult();
			var new = BinaryOpUGen.newDuringOptimisation(this.rate, '-', inputs[2], inputs[1]);
			this.replaceWith(new);
			result.addUGen(new, 1);
			^result;
		};
		// a * -1 + b => b - a
		if (inputs[1] == -1){
			var result = SynthDefOptimisationResult();
			var new = BinaryOpUGen.newDuringOptimisation(this.rate, '-', inputs[2], inputs[0]);
			this.replaceWith(new);
			result.addUGen(new, 1);
			^result;
		};

		// 1 * a + b => a + b
		if (inputs[0] == 1){
			var result = SynthDefOptimisationResult();
			var new = BinaryOpUGen.newDuringOptimisation(this.rate, '+', inputs[1], inputs[2]);
			this.replaceWith(new);
			result.addUGen(new, 1);
			^result;
		};
		// a * 1 + b => a + b
		if (inputs[1] == 1){
			var result = SynthDefOptimisationResult();
			var new = BinaryOpUGen.newDuringOptimisation(this.rate, '+', inputs[0], inputs[2]);
			this.replaceWith(new);
			result.addUGen(new, 1);
			^result;
		};


		// 2 * a + b => b
		if (inputs[0] == 0){
			var result = SynthDefOptimisationResult();
			var new1 = BinaryOpUGen.newDuringOptimisation(this.rate, '+', inputs[1], inputs[1]);
			var new2 = BinaryOpUGen.newDuringOptimisation(this.rate, '+', new1, inputs[2]);

			result.addUGen(new1, 1);
			result.addUGen(new2, 1);
			this.replaceWith(new2);
			^result;
		};
		// a * 2 + b => b
		if (inputs[1] == 0){
			var result = SynthDefOptimisationResult();
			var new1 = BinaryOpUGen.newDuringOptimisation(this.rate, '+', inputs[0], inputs[0]);
			var new2 = BinaryOpUGen.newDuringOptimisation(this.rate, '+', new1, inputs[2]);

			result.addUGen(new1, 1);
			result.addUGen(new2, 1);
			this.replaceWith(new2);
			^result;
		};

		// a * b + 0 => a * b
		if (inputs[2] == 0){
			var result = SynthDefOptimisationResult();
			var new = BinaryOpUGen.newDuringOptimisation(this.rate, '*', inputs[0], inputs[1]);
			this.replaceWith(new);
			result.addUGen(new, 1);
			^result;
		};


		^nil
	}
}

Sum3 : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { |in0, in1, in2| ^this.multiNew(nil, in0, in1, in2) }

	*new1 { |dummyRate, in0, in1, in2|
		var argArray, rate, sortedArgs;
		argArray = [in0, in1, in2];
		rate = argArray.rate;
		sortedArgs = argArray.sort {|a, b| a.rate < b.rate};
		^super.new1(rate, *sortedArgs)
	}

	optimise {
		var desc;

		// Sum3(a, b, c) + d => Sum4(a, b, c, d)
		desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '+' } };
		if(desc.isEmpty.not){
			var result = SynthDefOptimisationResult();
			desc.do { |add|
				var otherValues = add.inputs.select{|i| i !== this };
				// if Sum3(a, b, c) + Sum3(a, b, c) then the optimisation cannot be applied
				if (otherValues.isEmpty.not){
					var sum4 = Sum4.newDuringOptimisation(this.rate, inputs[0], inputs[1], inputs[2], otherValues[0]);
					add.replaceWith(with: sum4);
					add.tryDisconnect;
					result.addUGen(sum4, 1);
				}
			};
			result.returnNilIfEmpty !? { |r| ^r };
		};

		// Sum3(a, b, c) - a => (b + c)
		desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '-' } };
		if(desc.isEmpty.not){
			var result = SynthDefOptimisationResult();
			desc.do { |minus|
				var otherValues = minus.inputs.select{|i| i !== this };
				// if Sum3(a, b, c) - Sum3(a, b, c) then the optimisation cannot be applied (handled by BinaryOpUGen)
				if ((otherValues.size == 1) and: { inputs.includes(otherValues[0]) }) {
					var inputCopy = inputs.copy;
					var add;
					inputCopy.remove(otherValues[0]);
					add = BinaryOpUGen.newDuringOptimisation(this.rate, '+', inputCopy[0], inputCopy[1]);
					minus.replaceWith(with: add);
					minus.tryDisconnect;
					result.addUGen(add, 1);
				}
			};
			result.returnNilIfEmpty !? { |r| ^r };
		};

		if (inputs.every(_ === inputs[0])){
			var result = SynthDefOptimisationResult();
			var newMul = BinaryOpUGen.newDuringOptimisation(this.rate, '*', inputs[0], 3);
			this.replaceWith(with: newMul);
			this.tryDisconnect;
			result.addUGen(newMul, 1);
			^result
		};

		if (inputs.any(_ == 0)){
			var nonZero = inputs.select(_ != 0);
			case
			{ nonZero.size == 0 } {
				var result = SynthDefOptimisationResult();
				result.addConstantsDescendants(this.descendants);
				this.replaceWith(0);
				^result;
			}
			{ nonZero.size == 1 } {
				var result = SynthDefOptimisationResult();
				this.replaceWith(with: nonZero[0]);
				this.tryDisconnect;
				result.addUGen(nonZero[0], 1);
				^result
			}
			{ nonZero.size == 2 } {
				var result = SynthDefOptimisationResult();
				var new = BinaryOpUGen.newDuringOptimisation(this.rate, '+', nonZero[0], nonZero[1]);
				this.replaceWith(with: new);
				this.tryDisconnect;
				result.addUGen(new, 1);
				^result
			}
		};
		^nil;
	}
}

Sum4 : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg in0, in1, in2, in3;
		^this.multiNew(nil, in0, in1, in2, in3)
	}

	*new1 { arg dummyRate, in0, in1, in2, in3;
		var argArray = [in0, in1, in2, in3];
		var rate = argArray.rate;
		var sortedArgs = argArray.sort {|a b| a.rate < b.rate};

		^super.new1(rate, *sortedArgs)
	}

	optimise {
		var desc;

		// Sum4(a, b, c, d) - a => Sum3(b, c, d)
		desc = descendants.select { |d| d.isKindOf(BinaryOpUGen) and: { d.operator == '-' } };
		if(desc.isEmpty.not){
			var result = SynthDefOptimisationResult();
			desc.do { |minus|
				var otherValues = minus.inputs.select{|i| i !== this };
				// if Sum4(a, b, c, d) - Sum4(a, b, c, d) then the optimisation cannot be applied (handled by BinaryOpUGen)
				if ((otherValues.size == 1) and: { inputs.includes(otherValues[0]) }) {
					var inputCopy = inputs.copy;
					var sum3;
					inputCopy.remove(otherValues[0]);
					sum3 = Sum3.newDuringOptimisation(this.rate, inputCopy[0], inputCopy[1], inputCopy[2]);
					minus.replaceWith(with: sum3);
					minus.tryDisconnect;
					result.addUGen(sum3, 1);
				}
			};
			result.returnNilIfEmpty !? { |r| ^r };
		};

		// Sum4(a, a, a, a) => a * 4
		if (inputs.every(_ === inputs[0])){
			var result = SynthDefOptimisationResult();
			var newMul = BinaryOpUGen.newDuringOptimisation(this.rate, '*', inputs[0], 4);
			this.replaceWith(with: newMul);
			this.tryDisconnect;
			result.addUGen(newMul, 1);
			result.returnNilIfEmpty !? { |r| ^r };
		};

		// Sum4(a, 0, 0, 0) => a
		// Sum4(a, b, 0, 0) => a + b
		// Sum4(a, b, c, 0) => Sum3(a, b, c)
		if (inputs.any(_ == 0)){
			var result = SynthDefOptimisationResult();
			var nonZero = inputs.select(_ != 0);
			case
			{ nonZero.size == 0 } {
				var result = SynthDefOptimisationResult();
				result.addConstantsDescendants(this.descendants);
				this.replaceWith(0);
				^result;
			}
			{ nonZero.size == 1 } {
				var result = SynthDefOptimisationResult();
				this.replaceWith(with: nonZero[0]);
				this.tryDisconnect;
				result.addUGen(nonZero[0], 1);
				^result
			}
			{ nonZero.size == 2 } {
				var result = SynthDefOptimisationResult();
				var new = BinaryOpUGen.newDuringOptimisation(this.rate, '+', nonZero[0], nonZero[1]);
				this.replaceWith(with: new);
				this.tryDisconnect;
				result.addUGen(new, 1);
				^result
			}
			{ nonZero.size == 3 } {
				var result = SynthDefOptimisationResult();
				var new = Sum3.newDuringOptimisation(this.rate, nonZero[0], nonZero[1], nonZero[2]);
				this.replaceWith(with: new);
				this.tryDisconnect;
				result.addUGen(new, 1);
				^result
			}
		};
		^nil;
	}
}




