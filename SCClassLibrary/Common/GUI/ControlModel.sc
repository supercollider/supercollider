// ControlSpec - defines the range and curve of a control.

Spec {
	classvar <>specs;
	*initClass {
		specs = IdentityDictionary.new;
	}
	*add { arg name, args;
		var spec = args.asSpec;
		specs.put(name, spec);
		^spec
	}
	asSpec { ^this }
	defaultControl {
		^this.subclassResponsibility(thisMethod)
	}
	== { arg that;
		^this.compareObject(that)
	}
	hash {
		^this.instVarHash
	}
	findKey {
		^Spec.specs.findKeyForValue(this)
	}
	printOn { arg stream;
		var args;
		this.printClassNameOn(stream);
		args = this.storeArgs;
		if(args.notEmpty) {
			stream << "(" <<<* args << ")";
		}
	}

}

ControlSpec : Spec {
	var <minval, <maxval, <warp, <step, <>default, <>units, >grid;
	var <clipLo, <clipHi;

	*new { arg minval=0.0, maxval=1.0, warp='lin', step=0.0, default, units, grid;
		^super.newCopyArgs(minval, maxval, warp, step,
				default ? minval, units ? "", grid
			).init
	}

	*newFrom { arg similar;
		^this.new(similar.minval, similar.maxval, similar.warp.asSpecifier,
			similar.step, similar.default, similar.units)
	}

	storeArgs { ^[minval, maxval, warp.asSpecifier, step, default, units] }

	init {
		warp = warp.asWarp(this);
		clipLo = min(minval, maxval);
		clipHi = max(minval, maxval);
	}
	minval_ { arg v;
		minval = v;
		this.init;	// rechoose the constrainfunc
		this.changed(\minval);
	}
	maxval_ { arg v;
		maxval = v;
		this.init;
		this.changed(\maxval);
	}
	warp_ { arg w;
		warp = w.asWarp(this);
		this.changed(\warp);
	}
	step_ { arg s;
		step = s;
		this.changed(\step);
	}
	constrain { arg value;
		^value.asFloat.clip(clipLo, clipHi).round(step)
	}
	range { ^maxval - minval }
	ratio { ^maxval / minval }
	map { arg value;
		// maps a value from [0..1] to spec range
		^warp.map(value.clip(0.0, 1.0)).round(step);
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^warp.unmap(value.round(step).clip(clipLo, clipHi));
	}

	guessNumberStep {
			// first pass, good for linear warp
		var temp, numStep = this.range * 0.01;

			// for exponential warps, guess  again (hopefully educated)
		if (warp.asSpecifier == \exp) {
			temp = [minval, maxval].abs.minItem;
			^numStep = min(temp, numStep) * 0.1;
		};
			// others could go here.

		^numStep
	}

	grid { ^grid ?? {GridLines(this)} }

	looseRange { |data, defaultRange = 1.0|
		var newMin, newMax;
		data = data.flat;
		newMin = data.minItem;
		newMax = data.maxItem;
		if(newMin == newMax) {
			newMin = newMin - (defaultRange / 2);
			newMax = newMax + (defaultRange / 2);
		};
		# newMin, newMax = this.grid.looseRange(newMin,newMax);
		^this.copy.minval_(newMin).maxval_(newMax);
	}
	// can deprec
	calcRange { |data, defaultRange = 1.0|
		var newMin, newMax;
		data = data.flat;
		newMin = data.minItem;
		newMax = data.maxItem;
		if(newMin == newMax) {
			newMin = newMin - (defaultRange / 2);
			newMax = newMax + (defaultRange / 2);
		};
		^this.copy.minval_(newMin).maxval_(newMax);
	}

	normalize { |min, max|
		if(min.isNil) { min = if(this.hasZeroCrossing) { -1.0 } { 0.0 } };		if(max.isNil) { max = 1.0 };
		^this.copy.minval_(min).maxval_(max)
	}

	roundRange { |base = 10|
		var extent = absdif(minval, maxval);
		var r = 10 ** ((log10(extent) * log10(base)).trunc - 1);
		var newMin = minval.round(r);
		var newMax = maxval.roundUp(r);
		^this.copy.minval_(newMin).maxval_(newMax)
	}

	gridValues { |n = 20, min, max, base = 10|
		var val, exp;
		var low = if(min.notNil) { this.unmap(min) } { 0.0 };
		var high = if(max.notNil) { this.unmap(max) } { 1.0 };
		if(n < 1) { ^[] };
		^this.grid.getParams(low,high,numTicks:n).at('lines')
	}

	zoom { |ratio = 1|
		^this.copy.minval_(minval * ratio).maxval_(maxval * ratio)
	}

	shift { |amount = 1|
		^this.copy.minval_(minval + amount).maxval_(maxval + amount)
	}

	hasZeroCrossing {
		^minval.sign != maxval.sign
	}


	*initClass {
		Class.initClassTree(Warp);
		Class.initClassTree(Server);
		specs = specs.addAll([
			// set up some ControlSpecs for common mappings
			// you can add your own after the fact.

			\unipolar -> ControlSpec(0, 1),
			\bipolar -> ControlSpec(-1, 1, default: 0),

			\freq -> ControlSpec(20, 20000, \exp, 0, 440, units: " Hz"),
			\lofreq -> ControlSpec(0.1, 100, \exp, 0, 6, units: " Hz"),
			\midfreq -> ControlSpec(25, 4200, \exp, 0, 440, units: " Hz"),
			\widefreq -> ControlSpec(0.1, 20000, \exp, 0, 440, units: " Hz"),
			\phase -> ControlSpec(0, 2pi),
			\rq -> ControlSpec(0.001, 2, \exp, 0, 0.707),

			\audiobus -> ControlSpec(0, Server.default.options.numAudioBusChannels-1, step: 1),
			\controlbus -> ControlSpec(0, Server.default.options.numControlBusChannels-1, step: 1),

			\midi -> ControlSpec(0, 127, default: 64),
			\midinote -> ControlSpec(0, 127, default: 60),
			\midivelocity -> ControlSpec(1, 127, default: 64),

			\db -> ControlSpec(0.ampdb, 1.ampdb, \db, units: " dB"),
			\amp -> ControlSpec(0, 1, \amp, 0, 0),
			\boostcut -> ControlSpec(-20, 20, units: " dB",default: 0),

			\pan -> ControlSpec(-1, 1, default: 0),
			\detune -> ControlSpec(-20, 20, default: 0, units: " Hz"),
			\rate -> ControlSpec(0.125, 8, \exp, 0, 1),
			\beats -> ControlSpec(0, 20, units: " Hz"),

			\delay -> ControlSpec(0.0001, 1, \exp, 0, 0.3, units: " secs")
		]);
	}

	copy {
		^this.class.newFrom(this)
	}

}


// Warps specify the mapping from 0..1 and the control range

Warp {
	classvar <>warps;
	var <>spec;
	*new { arg spec;
		^super.newCopyArgs(spec.asSpec);
	}
	map { arg value; ^value }
	unmap { arg value; ^value }

	*asWarp { arg spec; ^this.new(spec) }
	asWarp { ^this }
	*initClass {
		// support Symbol-asWarp
		warps = IdentityDictionary[
			\lin -> LinearWarp,
			\exp -> ExponentialWarp,
			\sin -> SineWarp,
			\cos -> CosineWarp,
			\amp -> FaderWarp,
			\db -> DbFaderWarp,
			\linear -> LinearWarp,
			\exponential -> ExponentialWarp
		];
		// CurveWarp is specified by a number, not a Symbol
	}
	asSpecifier {
		^warps.findKeyForValue(this.class)
	}
	== { arg that;
		if(this === that,{ ^true; });
		if(that.class !== this.class,{ ^false });
		//^spec == that.spec
		^true
	}
	hash {
		^this.class.hash
	}

}

LinearWarp : Warp {
	map { arg value;
		// maps a value from [0..1] to spec range
		^value * spec.range + spec.minval
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^(value - spec.minval) / spec.range
	}
}

ExponentialWarp : Warp {
	// minval and maxval must both be non zero and have the same sign.
	map { arg value;
		// maps a value from [0..1] to spec range
		^(spec.ratio ** value) * spec.minval
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^log(value/spec.minval) / log(spec.ratio)
	}
}

CurveWarp : Warp {
	var a, b, grow, <curve;
	*new { arg spec, curve = -2;
		// prevent math blow up
		if (abs(curve) < 0.001, { ^LinearWarp(spec) });

		^super.new(spec.asSpec).init(curve);
	}
	init { arg argCurve;
		curve = argCurve;
		if(curve.exclusivelyBetween(-0.001,0.001),{
			curve = 0.001;
		});
		grow = exp(curve);
		a = spec.range / (1.0 - grow);
		b = spec.minval + a;
	}
	map { arg value;
		// maps a value from [0..1] to spec range
		^b - (a * pow(grow, value));
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^log((b - value) / a) / curve
	}
	asSpecifier { ^curve }

	== { arg that;
		^this.compareObject(that, [\curve])
	}

	hash {
		^this.instVarHash([\curve])
	}
}

CosineWarp : LinearWarp {
	map { arg value;
		// maps a value from [0..1] to spec range
		^super.map(0.5 - (cos(pi * value) * 0.5))
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^acos(1.0 - (super.unmap(value) * 2.0)) / pi
	}
}

SineWarp : LinearWarp {
	map { arg value;
		// maps a value from [0..1] to spec range
		^super.map(sin(0.5pi * value))
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^asin(super.unmap(value)) / 0.5pi
	}
}

FaderWarp : Warp {
	//  useful mapping for amplitude faders
	map { arg value;
		// maps a value from [0..1] to spec range
		^if(spec.range.isPositive) {
			value.squared * spec.range + spec.minval
		}{
				// formula can be reduced to (2*v) - v.squared
				// but the 2 subtractions would be faster
			(1 - (1-value).squared) * spec.range + spec.minval
		};
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^if(spec.range.isPositive) {
			((value - spec.minval) / spec.range).sqrt
		}{
			1 - sqrt(1 - ((value - spec.minval) / spec.range))
		}
	}
}

DbFaderWarp : Warp {
	//  useful mapping for amplitude faders
	map { arg value;
		// maps a value from [0..1] to spec range
		var	range = spec.maxval.dbamp - spec.minval.dbamp;
		^if(range.isPositive) {
			(value.squared * range + spec.minval.dbamp).ampdb
		}{
			((1 - (1-value).squared) * range + spec.minval.dbamp).ampdb
		}
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^if(spec.range.isPositive) {
			((value.dbamp - spec.minval.dbamp) / (spec.maxval.dbamp - spec.minval.dbamp)).sqrt
		}{
			1 - sqrt(1 - ((value.dbamp - spec.minval.dbamp) / (spec.maxval.dbamp - spec.minval.dbamp)))
		}
	}
}
