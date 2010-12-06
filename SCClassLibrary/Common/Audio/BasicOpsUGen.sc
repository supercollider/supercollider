
// These Unit Generators are instantiated by math operations on UGens

BasicOpUGen : UGen {
	var <operator;

//	writeName { arg file;
//		var name, opname;
//		name = this.class.name.asString;
//		opname = operator.asString;
//		file.putInt8(name.size + opname.size + 1);
//		file.putString(name);
//		file.putInt8(0);
//		file.putString(opname);
//	}
	operator_ { arg op;
		operator = op;
			// 'firstArg' exists in the server
			// but the 'specialIndex' primitive doesn't report the right value
			// admittedly this is a hack, but the hack was approved on sc-dev
		if(operator == 'firstArg') {
			specialIndex = 46
		} {
			specialIndex = operator.specialIndex;
		};
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

}

BinaryOpUGen : BasicOpUGen {
	*new { arg selector, a, b;
		^this.multiNew('audio', selector, a, b)
	}

	determineRate { arg a, b;
		if (a.rate == \demand, { ^\demand });
		if (b.rate == \demand, { ^\demand });
		if (a.rate == \audio, { ^\audio });
		if (b.rate == \audio, { ^\audio });
		if (a.rate == \control, { ^\control });
		if (b.rate == \control, { ^\control });
		^\scalar
	}
	*new1 { arg rate, selector, a, b;

		// eliminate degenerate cases
		if (selector == '*', {
			if (a == 0.0, { ^0.0 });
			if (b == 0.0, { ^0.0 });
			if (a == 1.0, { ^b });
			if (a == -1.0, { ^b.neg });
			if (b == 1.0, { ^a });
			if (b == -1.0, { ^a.neg });
		},{
		if (selector == '+', {
			if (a == 0.0, { ^b });
			if (b == 0.0, { ^a });
		},{
		if (selector == '-', {
			if (a == 0.0, { ^b.neg });
			if (b == 0.0, { ^a });
		},{
		if (selector == '/', {
			if (b == 1.0, { ^a });
			if (b == -1.0, { ^a.neg });
			if (b.rate == 'scalar', { ^a * b.reciprocal });
		})})})});

 		^super.new1(rate, selector, a, b)
	}

	init { arg theOperator, a, b;
		this.operator = theOperator;
		rate = this.determineRate(a, b);
		inputs = [a, b];
	}

	optimizeGraph {
		var a, b, muladd;
		#a, b = inputs;

		//this.constantFolding;

		if (operator == '+', {
			// create a MulAdd if possible.
			if (a.isKindOf(BinaryOpUGen) and: { a.operator == '*'
				and: { a.descendants.size == 1 }},
			{
				if (MulAdd.canBeMulAdd(a.inputs[0], a.inputs[1], b), {
					buildSynthDef.removeUGen(a);
					muladd = MulAdd.new(a.inputs[0], a.inputs[1], b);
				},{
				if (MulAdd.canBeMulAdd(a.inputs[1], a.inputs[0], b), {
					buildSynthDef.removeUGen(a);
					muladd = MulAdd.new(a.inputs[1], a.inputs[0], b)
				})});
			},{
			if (b.isKindOf(BinaryOpUGen) and: { b.operator == '*'
				and: { b.descendants.size == 1 }},
			{
				if (MulAdd.canBeMulAdd(b.inputs[0], b.inputs[1], a), {
					buildSynthDef.removeUGen(b);
					muladd = MulAdd.new(b.inputs[0], b.inputs[1], a)
				},{
				if (MulAdd.canBeMulAdd(b.inputs[1], b.inputs[0], a), {
					buildSynthDef.removeUGen(b);
					muladd = MulAdd.new(b.inputs[1], b.inputs[0], a)
				})});
			})});
			if (muladd.notNil, {
				synthDef.replaceUGen(this, muladd);
			});
		});
	}

	constantFolding {
		var a, b, aa, bb, cc, dd, temp, ac_ops, value;

		// associative & commutative operators
		ac_ops = #['+','*','min','max','&&','||'];

		if (ac_ops.includes(operator).not) { ^this };

		#a, b = inputs;
		if (a.isKindOf(BinaryOpUGen) and: { operator == a.operator
			and: { b.isKindOf(BinaryOpUGen) and: { operator == b.operator } }}) {
			#aa, bb = a.inputs;
			#cc, dd = b.inputs;
			if (aa.isKindOf(SimpleNumber)) {
				if (cc.isKindOf(SimpleNumber)) {
					b.inputs[0] = bb;
					this.inputs[0] = aa.perform(operator, cc);
					synthDef.removeUGen(a);
				}{
				if (dd.isKindOf(SimpleNumber)) {
					b.inputs[1] = bb;
					this.inputs[0] = aa.perform(operator, dd);
					synthDef.removeUGen(a);
				}}
			}{
			if (bb.isKindOf(SimpleNumber)) {
				if (cc.isKindOf(SimpleNumber)) {
					b.inputs[0] = aa;
					this.inputs[0] = bb.perform(operator, cc);
					synthDef.removeUGen(a);
				}{
				if (dd.isKindOf(SimpleNumber)) {
					b.inputs[1] = aa;
					this.inputs[0] = bb.perform(operator, dd);
					synthDef.removeUGen(a);
				}}
			}};

		};
		#a, b = inputs;
		if (a.isKindOf(BinaryOpUGen) and: { operator == a.operator }) {
			#aa, bb = a.inputs;
			if (b.isKindOf(SimpleNumber)) {
				if (aa.isKindOf(SimpleNumber)) {
					buildSynthDef.removeUGen(a);
					this.inputs[0] = aa.perform(operator, b);
					this.inputs[1] = bb;
					^this
				};
				if (bb.isKindOf(SimpleNumber)) {
					buildSynthDef.removeUGen(a);
					this.inputs[0] = bb.perform(operator, b);
					this.inputs[1] = aa;
					^this
				};
			};
			// percolate constants upward so that a subsequent folding may occur
			if (aa.isKindOf(SimpleNumber)) {
				this.inputs[1] = aa;
				a.inputs[0] = b;
			}{
			if (bb.isKindOf(SimpleNumber)) {
				this.inputs[1] = bb;
				a.inputs[1] = b;
			}};
		};
		#a, b = inputs;
		if (b.isKindOf(BinaryOpUGen) and: { operator == b.operator }) {
			#cc, dd = b.inputs;
			if (a.isKindOf(SimpleNumber)) {
				if (cc.isKindOf(SimpleNumber)) {
					buildSynthDef.removeUGen(b);
					this.inputs[0] = a.perform(operator, cc);
					this.inputs[1] = dd;
					^this
				};
				if (dd.isKindOf(SimpleNumber)) {
					buildSynthDef.removeUGen(b);
					this.inputs[0] = a.perform(operator, dd);
					this.inputs[1] = cc;
					^this
				};
			};
			// percolate constants upward so that a subsequent folding may occur
			if (cc.isKindOf(SimpleNumber)) {
				this.inputs[0] = cc;
				b.inputs[0] = a;
			}{
			if (dd.isKindOf(SimpleNumber)) {
				this.inputs[0] = dd;
				b.inputs[1] = a;
			}};
		};
		#a, b = inputs;
		if (a.isKindOf(SimpleNumber) and: { b.isKindOf(SimpleNumber) }) {
			synthDef.replaceUGen(this, a.perform(operator, b));
			synthDef.removeUGen(this);
		};
	}
}

MulAdd : UGen {
	*new { arg in, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, mul, add)
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

 		^super.new1(rate, in, mul, add)
	}
	init { arg in, mul, add;
		rate = in.rate;
		inputs = [in, mul, add];
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


