
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
	specialIndex { ^operator.specialIndex }
}

UnaryOpUGen : BasicOpUGen {	
	
	*new { arg selector, a;
		^this.multiNew('audio', selector, a)
	}
	
	init { arg theOperator, theInput;
		operator = theOperator;
		rate = theInput.rate;
		inputs = theInput.asArray;
	}

}

BinaryOpUGen : BasicOpUGen {		
	*new { arg selector, a, b;
		^this.multiNew('audio', selector, a, b)
	}
	
	determineRate { arg a, b;
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
		operator = theOperator;
		rate = this.determineRate(a, b);
		inputs = [a, b];
	}
	
	optimizeGraph {
		var a, b, muladd;
		#a, b = inputs;
		
		if (operator == '+', {
			// create a MulAdd if possible.
			if (a.isKindOf(BinaryOpUGen) && { a.operator == '*' }
				&& { a.descendants.size == 1 }, 
			{
				if (MulAdd.canBeMulAdd(a.inputs.at(0), a.inputs.at(1), b), {
					buildSynthDef.removeUGen(a);
					muladd = MulAdd.new(a.inputs.at(0), a.inputs.at(1), b);
				},{
				if (MulAdd.canBeMulAdd(a.inputs.at(1), a.inputs.at(0), b), {
					buildSynthDef.removeUGen(a);
					muladd = MulAdd.new(a.inputs.at(1), a.inputs.at(0), b)
				})});
			},{
			if (b.isKindOf(BinaryOpUGen) && { b.operator == '*' }
				&& { b.descendants.size == 1 }, 
			{
				if (MulAdd.canBeMulAdd(b.inputs.at(0), b.inputs.at(1), a), {
					buildSynthDef.removeUGen(b);
					muladd = MulAdd.new(b.inputs.at(0), b.inputs.at(1), a)
				},{
				if (MulAdd.canBeMulAdd(b.inputs.at(1), b.inputs.at(0), a), {
					buildSynthDef.removeUGen(b);
					muladd = MulAdd.new(b.inputs.at(1), b.inputs.at(0), a)
				})});
			})});
			if (muladd.notNil, {
				synthDef.replaceUGen(this, muladd);
			});
		});
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
			&& { mul.rate != \audio } 
			&& { add.rate != \audio }, 
		{ 
			^true 
		});
		^false
	}
}


