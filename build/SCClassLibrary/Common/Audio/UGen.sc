
UGen : AbstractFunction {
	classvar <>buildSynthDef; // the synth currently under construction
	var <>synthDef;
	var <>inputs;
	var <>rate = 'audio';
	
	var <>synthIndex = -1, <>specialIndex=0;
	
	var <>antecedents, <>descendants; // topo sorting
	
	// instance creation
	*new1 { arg rate ... args;
		if (rate.isKindOf(Symbol).not) { 
			^Error("rate must be Symbol. probably called 'new' instead of .ar or .kr") 
		};
		^super.new.rate_(rate).addToSynth.init( *args ) 
	}
	*newFromDesc { arg rate, numOutputs, inputs, specialIndex;
		^super.new.rate_(rate).inputs_(inputs).specialIndex_(specialIndex)
	}
 	*multiNew { arg ... args;
		^this.multiNewList(args);
	}
	
	*multiNewList { arg args;
		var size = 0, newArgs, results;
		args = args.asUGenInput;	
		args.do({ arg item; 
			(item.class == Array).if({ size = max(size, item.size) });
		});
		if (size == 0) { ^this.new1( *args ) };
		newArgs = Array.newClear(args.size);
		results = Array.newClear(size);
		size.do({ arg i;
			args.do({ arg item, j;
				newArgs.put(j, if (item.class == Array, { item.wrapAt(i) },{ item }));
			});
			results.put(i, this.multiNewList(newArgs));
		});
		^results
	}

 	init { arg ... theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
 	copy { 
 		// you can't really copy a UGen without disturbing the Synth. 
 		// Usually you want the same object. This makes .dup work
 		^this 
 	}
 	
 	madd { arg mul = 1.0, add = 0.0; 		
 		^MulAdd(this, mul, add);
 	}
 	range { arg lo = 0.0, hi = 1.0; 	
 		var mul, add;	
		if (this.signalRange == \bipolar, {
			mul = (hi - lo) * 0.5;
			add = mul + lo;
		},{
			mul = (hi - lo) ;
			add = lo;
		});
 		^MulAdd(this, mul, add);
 	}
 	exprange { arg lo = 0.01, hi = 1.0;
 		var rate = if(this.rate == \audio) { \audio } { \control };
		if (this.signalRange == \bipolar, {
			^LinExp.multiNew(rate, this, -1, 1, lo, hi)
		},{
			^LinExp.multiNew(rate, this, 0, 1, lo, hi)
		});
 	}
 	clip { arg lo,hi;
 		if(rate == \audio) {
 			^Clip.ar(this, lo, hi)
 		}{
 			^Clip.kr(this, lo, hi)
 		}
 	}
 	fold { arg lo,hi;
 		if(rate == \audio) {
 			^Fold.ar(this, lo, hi)
 		}{
 			^Fold.kr(this, lo, hi)
 		}
 	}
 	wrap { arg lo,hi;
 		if(rate == \audio) {
 			^Wrap.ar(this, lo, hi)
 		}{
 			^Wrap.kr(this, lo, hi)
 		}
 	}
 	
 	minNyquist { ^min(this, SampleRate.ir * 0.5) }
 	
 	lag { arg lagTime=0.1;
		^Lag.multiNew(this.rate, this, lagTime)
	}
	lag2 { arg lagTime=0.1;
		^Lag2.multiNew(this.rate, this, lagTime)
	}
	lag3 { arg lagTime=0.1;
		^Lag3.multiNew(this.rate, this, lagTime)
	}
	
	signalRange { ^\bipolar }
	@ { arg y; ^Point.new(this, y) } // dynamic geometry support
 	 	
	addToSynth {
		synthDef = buildSynthDef;
		if (synthDef.notNil, { synthDef.addUGen(this) });
	}
	
	collectConstants {
		inputs.do({ arg input;
			if (input.isNumber, { synthDef.addConstant(input.asFloat)  });
		}); 
	}
				
	isValidUGenInput { ^true }
	asUGenInput { ^this }
	numChannels { ^1 }
	
	
	checkInputs { ^this.checkValidInputs }
	checkValidInputs {
		inputs.do({arg in,i;
			var argName;
			if(in.isValidUGenInput.not,{
				argName = this.argNameForInputAt(i) ? i;
				^"arg: '" ++ argName ++ "' has bad input:" + in;
			})
		});
		^nil
	}
	checkSameRateAsFirstInput {
 		if (rate !== inputs.at(0).rate) { 
 			^("first input is not" + rate + "rate: " + inputs.at(0) + inputs.at(0).rate);
 		};
 		^this.checkValidInputs
 	}
 	
	argNameForInputAt { arg i;
		var method = this.class.class.findMethod(this.methodSelectorForRate);
		if(method.isNil or: {method.argNames.isNil},{ ^nil });
		^method.argNames.at(i + this.argNamesInputsOffset)
	}
	argNamesInputsOffset { ^1 }
	dumpArgs {
		" ARGS:".postln;
		inputs.do({ arg in,ini;
			("   " ++ (this.argNameForInputAt(ini) ? ini.asString)++":" + in + in.class).postln
		});
	}
	degreeToKey { arg scale, stepsPerOctave=12;
		^DegreeToKey.kr(scale, this, stepsPerOctave)
	}
	
	outputIndex { ^0 }
	
	// polling support
	poll { arg interval = 0.1, label = "UGen:";
		^SynthDef.wrap({
			var id, responder;
			id = this.hash & 0x7FFFFF;
			switch(this.rate, 
				\audio, {SendTrig.ar(Impulse.ar(interval.reciprocal), id, this)},
				\control, {SendTrig.kr(Impulse.kr(interval.reciprocal), id, this)}
			);
			responder = OSCresponderNode(nil,'/tr',{ arg time, rder, msg;
				if(msg[2] == id, { (label.asString + msg[3]).postln;});
			}).add;
			CmdPeriod.doOnce({ responder.remove });
			this;
		})
	}
	
	// PRIVATE
	// function composition
	composeUnaryOp { arg aSelector;
		^UnaryOpUGen.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, anInput;
		if (anInput.isValidUGenInput, {
			^BinaryOpUGen.new(aSelector, this, anInput)
		},{
			anInput.performBinaryOpOnUGen(aSelector, this);
		});
	}
	reverseComposeBinaryOp { arg aSelector, aUGen;
		^BinaryOpUGen.new(aSelector, aUGen, this)
	}
	composeNAryOp { arg aSelector, anArgList;
		^thisMethod.notYetImplemented
	}
	
	// complex support	
	
	asComplex { ^Complex.new(this, 0.0) }
	performBinaryOpOnComplex { arg aSelector, aComplex; ^aComplex.perform(aSelector, this.asComplex) }
	
	if { arg trueUGen, falseUGen;
		^(this * (trueUGen - falseUGen)) + falseUGen;
	}

	rateNumber {
		if (rate == \audio, { ^2 });
		if (rate == \control, { ^1 });
		if (rate == \demand, { ^3 });
		^0 // scalar
	}
	methodSelectorForRate {
		if(rate == \audio,{ ^\ar });
		if(rate == \control, { ^\kr });
		if(rate == \scalar, { 
			if(this.class.class.respondsTo(\ir),{
				^\ir
			},{
				^\new
			});
		});
		if(rate == \demand, { ^\new });
		^nil
	}
	writeInputSpec { arg file, synthDef;
		file.putInt16(synthIndex); 
		file.putInt16(this.outputIndex);
	}
	writeOutputSpec { arg file;
		file.putInt8(this.rateNumber);
	}
	writeOutputSpecs { arg file;
		this.writeOutputSpec(file);
	}
	numInputs { ^inputs.size }
	numOutputs { ^1 }
	
	name { 
		^this.class.name.asString;
	}
	writeDef { arg file;
		//[\WR, this.class.name, rate, this.numInputs, this.numOutputs].postln;
		file.putPascalString(this.name);
		file.putInt8(this.rateNumber);
		file.putInt16(this.numInputs);
		file.putInt16(this.numOutputs);
		file.putInt16(this.specialIndex);
		// write wire spec indices.
		inputs.do({ arg input; 
			input.writeInputSpec(file, synthDef);
		});
		this.writeOutputSpecs(file);
		//[this.class.name, file.length].postln;
	}

///////////////////////////////////////////////////////////////

	initTopoSort {
		inputs.do({ arg input;
			if (input.isKindOf(UGen), {
				antecedents.add(input.source);
				input.source.descendants.add(this);
			});
		})
	}
	
	makeAvailable {
		if (antecedents.size == 0, {
			synthDef.available = synthDef.available.add(this);
		});
	}
	
	removeAntecedent { arg ugen;
		antecedents.remove(ugen);
		this.makeAvailable;
	}
		
	schedule { arg outStack;
		descendants.reverseDo({ arg ugen;
			ugen.removeAntecedent(this);
		});
		^outStack.add(this);
	}
		
	optimizeGraph {}
	
	dumpName {
		^synthIndex.asString ++ "_" ++ this.class.name.asString
	}
}

MultiOutUGen : UGen {
	// a class for UGens with multiple outputs
	var <channels;

	*newFromDesc { arg rate, numOutputs, inputs;
		^super.new.rate_(rate).inputs_(inputs).initOutputs(numOutputs, rate)
	}

	initOutputs { arg numChannels, rate;
		channels = Array.fill(numChannels, { arg i; 
			OutputProxy(rate, this, i); 
		});
		if (numChannels == 1, {
			^channels.at(0)
		});
		^channels
	}
	
	numOutputs { ^channels.size }
	writeOutputSpecs { arg file;
		channels.do({ arg output; output.writeOutputSpec(file); });
	}
	synthIndex_ { arg index;
		synthIndex = index;
		channels.do({ arg output; output.synthIndex_(index); });
	}

}	

OutputProxy : UGen {
	var <>source, <>outputIndex;
	*new { arg rate, itsSourceUGen, index;
		^super.new1(rate, itsSourceUGen, index)
	}
	addToSynth {
		synthDef = buildSynthDef;
	}
	init { arg argSource, argIndex;
		source = argSource;
		outputIndex = argIndex;
		synthIndex = source.synthIndex;
	}
	
	dumpName {
		^this.source.dumpName ++ "[" ++ outputIndex ++ "]"
	}
}
