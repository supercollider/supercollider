// UGenBuiltInMethods contains common methods (usually mathematical) that UGen implements.
// This helps keep the UGen class easier to read.

UGenBuiltInMethods : AbstractFunction {
	rate { this.subclassResponsibility(thisMethod) }

	signalRange { ^\bipolar }

	madd { |mul = 1.0, add = 0.0| ^ this * mul + add; }
	unipolar { |mul = 1| ^this.range(0, mul) }
	bipolar { |mul = 1| ^this.range(mul.neg, mul) }
	degrad { ^this * 0.01745329251994329547 /* degree * (pi/180) */ }
	raddeg { ^this * 57.29577951308232286465 /* radian * (180/pi) */ }
	minNyquist { ^min(this, SampleRate.ir * 0.5) }
	range { |lo = 0.0, hi = 1.0|
		^if (this.signalRange == \bipolar) {
			var mul = (hi - lo) * 0.5;
			var add = mul + lo;
			this * mul + add;
		} {
			this * (hi - lo) + lo
		}
	}
	exprange { |lo = 0.01, hi = 1.0|
		^this.linexp((this.signalRange == \bipolar).if(-1, 0), 1, lo, hi, nil)
	}
	curverange { |lo = 0.00, hi = 1.0, curve = -4|
		^this.lincurve((this.signalRange == \bipolar).if(-1, 0), 1, lo, hi, curve, nil)
	}
	clip { |lo = 0.0, hi = 1.0|
		^if(this.rate == \demand){
			max(lo, min(hi, this))
		}{
			Clip.perform(Clip.methodSelectorForRate(this.rate), this, lo, hi)
		}
	}
	fold { |lo = 0.0, hi = 0.0|
		if(this.rate == \demand) { this.notYetImplemented(thisMethod) };
		^Fold.perform(Fold.methodSelectorForRate(this.rate), this, lo, hi)
	}
	wrap { |lo = 0.0, hi = 1.0|
		if(this.rate == \demand) { this.notYetImplemented(thisMethod) };
		^Wrap.perform(Wrap.methodSelectorForRate(this.rate), this, lo, hi)
	}


	blend { |that, blendFrac = 0.5|
		var pan;
		if (this.rate == \demand || that.rate == \demand) { this.notYetImplemented(thisMethod) };

		pan = blendFrac.linlin(0.0, 1.0, -1, 1);

		if (this.rate == \audio) { ^XFade2.ar(this, that, pan) };

		if (that.rate == \audio) { ^XFade2.ar(that, this, pan.neg) };

		^LinXFade2.perform(LinXFade2.methodSelectorForRate(this.rate), this, that, pan)
	}


	lag { |t1 = 0.1, t2|
		^if(t2.isNil) {
			Lag.multiNew(this.rate, this, t1)
		} {
			LagUD.multiNew(this.rate, this, t1, t2)
		}
	}
	lag2 { |t1 = 0.1, t2|
		^if(t2.isNil) {
			Lag2.multiNew(this.rate, this, t1)
		} {
			Lag2UD.multiNew(this.rate, this, t1, t2)
		}
	}
	lag3 { arg t1 = 0.1, t2;
		^if(t2.isNil) {
			Lag3.multiNew(this.rate, this, t1)
		} {
			Lag3UD.multiNew(this.rate, this, t1, t2)
		}
	}

	lagud { arg lagTimeU=0.1, lagTimeD=0.1;
		^LagUD.multiNew(this.rate, this, lagTimeU, lagTimeD)
	}
	lag2ud { arg lagTimeU=0.1, lagTimeD=0.1;
		^Lag2UD.multiNew(this.rate, this, lagTimeU, lagTimeD)
	}
	lag3ud { arg lagTimeU=0.1, lagTimeD=0.1;
		^Lag3UD.multiNew(this.rate, this, lagTimeU, lagTimeD)
	}

	varlag { arg time=0.1, curvature=0, warp=5, start;
		^VarLag.multiNew(this.rate, this, time, curvature, warp, start)
	}

	slew { arg up = 1, down = 1;
		^Slew.multiNew(this.rate, this, up, down)
	}

	prune { arg min, max, type;
		switch(type,
			\minmax, { ^this.clip(min, max) },
			\min, { ^this.max(min) },
			\max, { ^this.min(max) }
		);
		^this
	}

	snap { arg resolution = 1.0, margin = 0.05, strength = 1.0;
		var round = round(this, resolution);
		var diff = round - this;
		^Select.multiNew(this.rate, abs(diff) < margin, this, this + (strength * diff));
	}

	softRound { arg resolution = 1.0, margin = 0.05, strength = 1.0;
		var round = round(this, resolution);
		var diff = round - this;
		^Select.multiNew(this.rate, abs(diff) > margin, this, this + (strength * diff));
	}

	linlin { arg inMin, inMax, outMin, outMax, clip = \minmax;
		if (this.rate == \audio) {
			^LinLin.ar(this.prune(inMin, inMax, clip), inMin, inMax, outMin, outMax)
		} {
			^LinLin.kr(this.prune(inMin, inMax, clip), inMin, inMax, outMin, outMax)
		}
	}

	linexp { arg inMin, inMax, outMin, outMax, clip = \minmax;
		^LinExp.multiNew(this.rate, this.prune(inMin, inMax, clip),
			inMin, inMax, outMin, outMax)
	}
	explin { arg inMin, inMax, outMin, outMax, clip = \minmax;
		^(log(this.prune(inMin, inMax, clip)/inMin))
		/ (log(inMax/inMin)) * (outMax-outMin) + outMin; // no separate ugen yet
	}
	expexp { arg inMin, inMax, outMin, outMax, clip = \minmax;
		^pow(outMax/outMin, log(this.prune(inMin, inMax, clip)/inMin)
			/ log(inMax/inMin)) * outMin;
	}

	lincurve { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		var grow, a, b, scaled, curvedResult;
		if (curve.isNumber and: { abs(curve) < 0.125 }) {
			^this.linlin(inMin, inMax, outMin, outMax, clip)
		};
		grow = exp(curve);
		a = outMax - outMin / (1.0 - grow);
		b = outMin + a;
		scaled = (this.prune(inMin, inMax, clip) - inMin) / (inMax - inMin);

		curvedResult = b - (a * pow(grow, scaled));

		if (curve.rate == \scalar) {
			^curvedResult
		} {
			^Select.perform(this.methodSelectorForRate, abs(curve) >= 0.125, [
				this.linlin(inMin, inMax, outMin, outMax, clip),
				curvedResult
			])
		}
	}

	curvelin { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		var grow, a, b, scaled, linResult;
		if (curve.isNumber and: { abs(curve) < 0.125 }) {
			^this.linlin(inMin, inMax, outMin, outMax, clip)
		};
		grow = exp(curve);
		a = inMax - inMin / (1.0 - grow);
		b = inMin + a;

		linResult = log( (b - this.prune(inMin, inMax, clip)) / a ) * (outMax - outMin) / curve + outMin;

		if (curve.rate == \scalar) {
			^linResult
		} {
			^Select.perform(this.methodSelectorForRate, abs(curve) >= 0.125, [
				this.linlin(inMin, inMax, outMin, outMax, clip),
				linResult
			])
		}
	}

	bilin { arg inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax;
		^Select.perform(this.methodSelectorForRate, this < inCenter,
			[
				this.linlin(inCenter, inMax, outCenter, outMax, clip),
				this.linlin(inMin, inCenter, outMin, outCenter, clip)
			]
		)
	}

	moddif { |that = 0.0, mod = 1.0|
		^ModDif.multiNew(this.rate, this, that, mod)
	}

	binaryValue { ^this.sign.max(0) }

	// Note that this differs from |==| for other AbstractFunctions
	// Other AbstractFunctions write '|==|' into the compound function
	// for the sake of their 'storeOn' (compile string) representation.
	// For UGens, scsynth does not support |==| (same handling --> error).
	// So here, we use '==' which scsynth does understand.
	// Also, BinaryOpUGen doesn't write a compile string.
	|==| { |that|
		^this.composeBinaryOp('==', that)
	}
	prReverseLazyEquals { |that|
		// commutative, so it's OK to flip the operands
		^this.composeBinaryOp('==', that)
	}

	sanitize {
		^Sanitize.perform(this.methodSelectorForRate, this);
	}

	@ { arg y; ^Point.new(this, y) } // dynamic geometry support

	poll { |trig = 10, label, trigid = -1| ^Poll(trig, this, label, trigid) }

	dpoll { |label, run = 1, trigid = -1| ^Dpoll(this, label, run, trigid) }

	checkBadValues { |id = 0, post = 2|
		// add the UGen to the tree but keep "this" as the output
		CheckBadValues.perform(this.methodSelectorForRate, this, id, post);
	}

	degreeToKey { | scale, stepsPerOctave = 12| ^DegreeToKey.kr(scale, this, stepsPerOctave) }

	// complex support
	asComplex { ^Complex.new(this, 0.0) }
	performBinaryOpOnComplex { |aSelector, aComplex| ^aComplex.perform(aSelector, this.asComplex) }

	if { |trueUGen, falseUGen| ^(this * (trueUGen - falseUGen)) + falseUGen }

	rateNumber {
		if (this.rate == \audio, { ^2 });
		if (this.rate == \control, { ^1 });
		if (this.rate == \demand, { ^3 });
		^0 // scalar
	}
	methodSelectorForRate {
		^switch (this.rate)
		{ \audio } { \ar }
		{ \control } { \kr }
		{ \demand } { \new }
		{ \scalar } { if(this.class.respondsTo(\ir)) { \ir }{ \new } }
		{ nil }
	}

	// PRIVATE ~ ish
	// function composition
	composeUnaryOp { |aSelector| ^UnaryOpUGen.new(aSelector, this) }
	reverseComposeBinaryOp { |aSelector, aUGen| ^BinaryOpUGen.new(aSelector, aUGen, this) }
	composeNAryOp { |aSelector, anArgList| ^thisMethod.notYetImplemented }
	composeBinaryOp { |aSelector, anInput|
		if (anInput.isValidUGenInput) {
			^BinaryOpUGen.new(aSelector, this, anInput)
		} {
			^anInput.performBinaryOpOnUGen(aSelector, this);
		}
	}
}

UGen : UGenBuiltInMethods {
	classvar <>buildSynthDef; // the synth currently under construction

	var <>synthDef; // The Current active synthdef.
	var <>inputs;
	var <>rate = 'audio';

	// The order in the synthdef, not set until the synthdef compilation is complete.
	var <>synthIndex = -1;

	var <>specialIndex = 0; // TODO: what does this do?

	// Graph edges. Set in initEdges due to weird issue with init.
	var <>antecedents, <>descendants, <>weakAntecedents, <>weakDescendants;
	var <depth = 0; // How many children are above it in the graph.

	///////////////// CONSTRUCTORS

	*newDuringOptimisation { |rate ...args|
		var ret = this.performList(\new1, rate, args);
		UGen.prInitEdgesRecursive(ret);
		^ret;
	}

	*multiNewDuringOptimisation { |...args|
		var ret = this.perform(\multiNew, *args);
		UGen.prInitEdgesRecursive(ret);
		^ret;
	}

	*new1 { |rate ... args|
		// If you override this method, you MUST ensure that the inputs are set before calling addToSynth.
		if (rate.isKindOf(Symbol).not) { Error("rate must be Symbol.").throw };
		^super.new.rate_(rate).inputs_(args).addToSynth.init(*args)
	}

	*newFromDesc { |rate, numOutputs, inputs, specialIndex|
		if (rate.isKindOf(Symbol).not) { Error("rate must be Symbol.").throw };
		^super.new.rate_(rate).specialIndex_(specialIndex).inputs_(inputs)
	}

	*multiNew { |... args| ^this.multiNewList(args) }

	*multiNewList { arg args;
		var size = 0, newArgs, results;
		args = args.asUGenInput(this);
		args.do{ |item| if (item.class == Array) { size = max(size, item.size) } };
		if (size == 0) {
			^this.new1( *args )
		};
		newArgs = Array.newClear(args.size);
		results = Array.newClear(size);
		size.do{ |i|
			args.do{ |item, j|
				newArgs.put(j, if (item.class == Array, { item.wrapAt(i) }, { item }) );
			};
			results.put(i, this.multiNewList(newArgs));
		};
		^results
	}


	// Other classes override init and return different classes,
	//    this causes all manner of headaches in the constructors,  be warned!
	init { |... theInputs| inputs = theInputs }


	///////////////// OVERIDEABLE INTERFACE FOR UGEN AUTHORS

	////// REQUIRED meta-info. All UGens should specify these three methods.
	////// Defaults are provided here, but each UGen should specifiy them to be clear.
	// 1. Return an Array of zero or more UGenResourceManagers, or nil if entering panic mode (see UGenResourceManager).
	// Maintains IO ordering under topological sort.
	resourceManagers { ^nil	}

	// 2. Outputs to buffer, bus, sends a message, or does something else observable.
	// Will be deleted if false and doesn't connect to a UGen that has an observable effect (dead code elimination).
	hasObservableEffect { ^true }

	// 3. Whether identical calls can be replaced by each other (common subexpression elimination).
	// E.g. in `var f = (a + b); var g = (a + b);` g can be deleted and replaced by f.
	// Importantly, the weak (IO/state) ordering is factored into whether a call is identical.
	// UGens that rely on randomness should set this to be false.
	canBeReplacedByIdenticalCall { ^false }

	////// Other methods for UGen authors.

	// Graph optimisations, replace patterns of UGens with others, optional.
	// This method should ONLY look at inputs (direct antecedents) but may look at all descendants.
	// The optimiser runs from output to input, walking 'up' the graph.
	// It must return a SynthDefOptimisationResult or a nil if no optimisation has occurred.
	// No attempt to delete UGens should be made, unless they have an observable effect (like Out),
	//    in which case it should be added to the result as an observableUGenReplacement.
	// To remove UGens without an observable effect, do nothing, the sort removes dead code automatically.
	// The depth of the descendants that have been removed should be returned in the result,
	//    this will force the optimiser to re-evaluate those descendants before continuing upwards.
	optimise { ^nil }


	// Attempt to convert inputs. For example, turning the scalar '0' into DC.ar(0).
	// If an invalid state is found, one may throw, or catch it in checkInputs (or both).
	coerceInputs { }

	// Choose a input validation strategy, see below for options.
	// Should not change the UGen, only throw when a mistake has been found.
	checkInputs { ^this.checkValidInputs }

	// Called once the synthdef has finished compiling and this UGen is present in the graph.
	onFinialisedSynthDef { }

	///////////////// HELPER METHODS FOR HAS OBSERVABLE EFFECT

	// Helper for hasObservableEffect when there is a doneAction
	implHasObservableEffectViaDoneAction { |index|
		^inputs.at(index).isNumber.not or: { inputs.at(index) != Done.none }
	}

	///////////////// HELPER METHODS FOR OPTIMISE

	replaceWith { |with|
		var withNoOutputProxy = if (with.isKindOf(OutputProxy)) { with.source } { with };
		if (this === with) {
			Error("Cannot call replaceWith on this").throw
		};
		descendants.reverse.do { |d|
			var indexes = [];
			d.inputs.do { |in, i|
				in = if(in.isKindOf(OutputProxy)) { in.source } { in };
				if (in === this) {
					indexes = indexes.add(i);
				}
			};
			indexes.reverseDo { |i|
				d.replaceInputAt(index: i, with: withNoOutputProxy)
			}
		};
		this.descendants = [];
		if (with.isKindOf(UGen)){
			with.weakDescendants = with.weakDescendants.addAll(this.weakDescendants);
			with.weakAntecedents = with.weakAntecedents.addAll(this.weakAntecedents);
			with.weakDescendants.removeIdenticalDuplicates;
			with.weakAntecedents.removeIdenticalDuplicates;
		};
		this.weakDescendants.do{ |w| w.weakAntecedents.remove(this) };
		this.weakAntecedents.do{ |w| w.weakDescendants.remove(this) };
		this.inputs.do{ |i|
			case
			{ i.isKindOf(OutputProxy)} {
				i.source.descendants.remove(this);
			}
			{ i.isKindOf(UGen) } {
				i.descendants.remove(this)
			}
		};
		this.inputs = [];
		this.antecedents = [];
		this.weakDescendants = [];
		this.weakAntecedents = [];
	}

	// This is unnecessary to call, but offers a small performance improvement during optimisation.
	tryDisconnect {
		if (descendants.isEmpty and: {this.hasObservableEffect.not}){
			this.inputs.do {|i| if(i.isKindOf(UGen)){ i.descendants.remove(this) } };
			this.inputs = [];
			this.descendants = [];
			this.antecedents = [];
			if(this.hasObservableEffect.not){
				buildSynthDef.children.remove(this)
			};
		}
	}

	///////////////// HELPER METHODS FOR CHECKINPUTS

	checkValidInputs {
		inputs.do { |in, i|
			if (in.isValidUGenInput.not) {
				^"arg: '" ++ (this.argNameForInputAt(i) ? i) ++ "' has bad input:" + in
			}
		};
		^nil
	}

	checkNInputs { |n|
		if (rate == 'audio') {
			n.do { |i|
				if (inputs.at(i).rate != 'audio') {
					^("input " ++ i ++ " is not audio rate: " + inputs.at(i) + inputs.at(0).rate)
				}
			}
		};
		^this.checkValidInputs
	}

	checkSameRateAsFirstInput {
		if (rate !== inputs.at(0).rate) {
			^("first input is not" + rate + "rate: " + inputs.at(0) + inputs.at(0).rate)
		};
		^this.checkValidInputs
	}

	///////////////// SHORT MISC METHODS

	isValidUGenInput { ^true }
	asUGenInput { ^this }
	asControlInput { Error("can't set a control to a UGen").throw }
	numChannels { ^1 }
	copy { ^this }
	outputIndex { ^0 } // TODO: what does this do?
	writesToBus { ^false } // TODO: what does this do?
	isUGen { ^true }  // TODO: duh, why is this needed?
	name { ^this.class.name.asString }
	nameForDisplay { ^this.name.asSymbol }
	dumpName { ^"%_%".format(synthIndex, this.name) }
	getIdenticalInputs { ^[rate, inputs, weakAntecedents, weakDescendants] }
	//asString { ^this.dumpName }
	//printOn { |stream| stream << this.asString }
	numInputs { ^inputs.size }
	numOutputs { ^1 }

	// This method should only be overriden in OutputProxy.
	// It is fragile so don't override this as a UGen author.
	addToSynth { (synthDef = buildSynthDef) !? _.addUGen(this) }


	///////////////// GRAPH FUNCTIONALITY

	// `a.createConnectionTo(ugen: b)` creates the edge a -> b
	createConnectionTo {|ugen|
		this.descendants = this.descendants.add(ugen);
		ugen.antecedents = ugen.antecedents.add(this);
	}

	// Replaces an input at index with.
	replaceInputAt { |index, with|
		var old = inputs[index];

		if (old.isKindOf(UGen)) {
			old.descendants.remove(this)
		};
		this.antecedents.remove(old);
		inputs[index] = with;
		if(with.isKindOf(UGen)) {
			with.createConnectionTo(this)
		};
	}

	// Creates a weak edge between two ugens. Weak edges are used to indicate IO and other resource orderings.
	createWeakConnectionTo { |ugen|
		ugen = if (ugen.isKindOf(OutputProxy)) { ugen.source } { ugen };
		this.weakDescendants = this.weakDescendants.add(ugen);
		ugen.weakAntecedents = ugen.weakAntecedents.add(this);
	}

	// This is called inside the topological sort.
	// It can't be called sooner because the optimisations changes things.
	sortAntecedents {
		// Could be nil.
		antecedents = antecedents.asArray;
		descendants = descendants.asArray;
		weakAntecedents = weakAntecedents.asArray;
		weakDescendants = weakDescendants.asArray;

		antecedents.sort{ |l, r| if(l.depth != r.depth) { l.depth > r.depth } { l.synthIndex < r.synthIndex } };
		weakAntecedents.sort{ |l, r| if(l.depth != r.depth) { l.depth > r.depth } { l.synthIndex < r.synthIndex } };
	}

	getAllDescendantsAtLevel { |level| // Level should be greater than 1.
		if(level <= 1) {
			^this.descendants
		} {
			^this.descendants.inject(this.descendants, {|arr, d| arr ++ d.getAllDescendantsAtLevel(level - 1) })
		}
	}

	// Edges cannot be made as we go along because the init method is overridden in some classes to return a class that IS NOT a UGen.
	// To fix this, you'd have to change Control and OutputProxy.
	// Do not override this method.
	initEdges {
		depth = 0;
		antecedents = antecedents.asArray; // Could be nil.
		descendants = descendants.asArray;
		weakAntecedents = weakAntecedents.asArray;
		weakDescendants = weakDescendants.asArray;
		inputs.do{ |input|
			if (input.isKindOf(UGen)) {
				antecedents = antecedents.add(input.source);
				input.source.descendants = input.source.descendants.add(this);
				depth = max(depth, input.depth + 1);
			}
		};
		weakAntecedents.do{ |a| depth = max(depth, a.depth + 1) };
		^this
	}


	///////////////// OTHER

	// Helper accessor for inputs
	argNamesInputsOffset { ^1 } // ignores first input
	argNameForInputAt { arg i;
		var method = this.class.class.findMethod(this.methodSelectorForRate);
		if(method.isNil or: {method.argNames.isNil},{ ^nil });
		^method.argNames.at(i + this.argNamesInputsOffset)
	}

	dumpArgs {
		" ARGS:".postln;
		inputs.do{ |in, index|
			("   " ++ (this.argNameForInputAt(index) ? index.asString) ++ ":" + in + in.class).postln
		};
	}

	// Stores all constants in the synthdef so they can be deduplicated
	collectConstants {
		inputs.select(_.isNumber).do{ |input| synthDef.addConstant(input.asFloat) }
	}

	///////////////// WRITING TO FILE

	writeInputSpec { |file, synthDef|
		file.putInt32(synthIndex);
		file.putInt32(this.outputIndex);
	}
	writeOutputSpec { |file| file.putInt8(this.rateNumber) }
	writeOutputSpecs { |file| this.writeOutputSpec(file) }
	writeDef { arg file;
		try {
			file.putPascalString(this.name);
			file.putInt8(this.rateNumber);
			file.putInt32(this.numInputs);
			file.putInt32(this.numOutputs);
			file.putInt16(this.specialIndex);
			// write wire spec indices.
			inputs.do({ arg input;
				input.writeInputSpec(file, synthDef);
			});
			this.writeOutputSpecs(file);
		} {
			arg e;
			Error("UGen: could not write def: %".format(e.what())).throw;
		}
	}

	///////////////// CLASS METHODS HELPERS
	*methodSelectorForRate { |rate|
		^switch (rate)
		{ \audio } { \ar }
		{ \control } { \kr }
		{ \demand } { \new }
		{ \scalar } { if(this.respondsTo(\ir)) { \ir }{ \new } }
		{ nil }
	}

	*replaceZeroesWithSilence { arg array;
		// this replaces zeroes with audio rate silence.
		// sub collections are deep replaced
		var numZeroes, silentChannels, pos = 0;

		numZeroes = array.count(_ == 0.0);
		if (numZeroes == 0) { ^array };

		silentChannels = Silent.ar(numZeroes).asCollection;
		array.do{ |item, i|
			var res;
			if (item == 0.0) {
				array.put(i, silentChannels.at(pos));
				pos = pos + 1;
			} {
				if(item.isSequenceableCollection) {
					res = this.replaceZeroesWithSilence(item);
					array.put(i, res);
				};
			};
		};
		^array;
	}

	*prInitEdgesRecursive { |ugen|
		if (ugen.isKindOf(UGen)) { 	^ugen.initEdges };
		if(ugen.isKindOf(Collection)) { ^ugen.do(UGen.prInitEdgesRecursive(_)) };
		if (ugen.isKindOf(Number)) { ^nil };
		Error("Expected a UGen, Number, or Collection thereof, recieved a %".format(ugen)).throw
	}
}

// Don't use, instead specify these manual.
PureUGen : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }
}

MultiOutUGen : UGen {
	// a class for UGens with multiple outputs
	var <channels;

	*newFromDesc { |rate, numOutputs, inputs| ^super.new.rate_(rate).inputs_(inputs).initOutputs(numOutputs, rate) }

	initOutputs { |numChannels, rate|
		if(numChannels.isInteger.not or: { numChannels < 1 }, {
			Error("%: numChannels must be a nonzero positive integer, but received (%)."
				.format(this, numChannels)).throw
		});
		channels = Array.fill(numChannels, { arg i;
			OutputProxy(rate, this, i);
		});
		if (numChannels == 1, {
			^channels.at(0)
		});
		^channels
	}

	numOutputs { ^channels.size }

	writeOutputSpecs { |file|
		channels.do{ |output| output.writeOutputSpec(file) }
	}

	synthIndex_ { |index|
		synthIndex = index;
		channels.do{ |output| output.synthIndex_(index) };
	}
}

// Don't use, instead specify these manual.
PureMultiOutUGen : MultiOutUGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }
}

OutputProxy : UGen {
	var <>source, <>outputIndex, <>name;

	*new { |rate, itsSourceUGen, index| ^super.new1(rate, itsSourceUGen, index) }

	addToSynth { synthDef = buildSynthDef }

	init { arg argSource, argIndex;
		super.init(argSource, argIndex);
		source = argSource;
		outputIndex = argIndex;
		synthIndex = source.synthIndex;
	}

	dumpName { ^this.source.dumpName ++ "[" ++ outputIndex ++ "]" }

	controlName {
		var counter = 0, index = 0;

		this.synthDef.children.do{ |ugen|
			if(this.source.synthIndex == ugen.synthIndex) {
				index = counter + this.outputIndex
			};
			if(ugen.isKindOf(Control)){
				counter = counter + ugen.channels.size
			};
		};

		^synthDef.controlNames.detect{ |c| c.index == index }
	}

	spec_{ |spec|
		this.controlName !? { this.controlName.spec = spec } ?? {
			Error("Cannot set spec on a non-Control").throw
		}
	}
}
