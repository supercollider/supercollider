
// ControlSpec - defines the range and curve of a control.

Spec {
	classvar <>specs;
	*initClass {
		specs = IdentityDictionary.new;
	}
	asSpec { ^this }
	defaultControl {
		^thisMethod.subclassResponsibility
	}
}

ControlSpec : Spec {
	var <>minval, <>maxval, <>warp, <>step, <>default, <>units;
	
	*new { arg minval=0.0, maxval=1.0, warp='lin', step=0.0, default, units;
		^super.newCopyArgs(minval, maxval, warp, step, 
				default ? minval, units ? ""
			).init
	}
	// make Warps independant of spec
	//storeArgs { ^[minval,maxval,warp,step,default,units] }
	init { 
		warp = warp.asWarp(this);
	}
	
	constrain { arg value;
		^value.clip(minval, maxval).round(step)
	}
	range { ^maxval - minval }
	ratio { ^maxval / minval }
	map { arg value;
		// maps a value from [0..1] to spec range
		^warp.map(value.clip(0.0, 1.0)).round(step);
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^warp.unmap(value.clip(minval, maxval).round(step));
	}
	
	*initClass {
		specs = specs.addAll([
			// set up some ControlSpecs for common mappings
			// you can add your own after the fact.
			
			\unipolar -> ControlSpec(0, 1),
			\bipolar -> ControlSpec(-1, 1, default: 0),

			\freq -> ControlSpec(20, 20000, \exp, 0, 440, units: " Hz"),
			\lofreq -> ControlSpec(0.1, 100, \exp, 0, 6, units: " Hz"),
			\widefreq -> ControlSpec(0.1, 20000, \exp, 0, 440, units: " Hz"),
			\phase -> ControlSpec(0, 2pi),
			\rq -> ControlSpec(0.001, 2, \exp, 0, 0.707),

			\audiobus -> ControlSpec(0, 128, step: 1),
			\controlbus -> ControlSpec(0, 4096, step: 1),

			\midi -> ControlSpec(0, 127, default: 64),
			\midinote -> ControlSpec(0, 127, default: 60),
			\midivelocity -> ControlSpec(1, 127, default: 64),
			
			\db -> ControlSpec(0.ampdb, 1.ampdb, \db, units: " dB"),
			\amp -> ControlSpec(0, 1, \amp, 0, 0),
			\boostcut -> ControlSpec(-20, 20, units: " dB"),
			
			\pan -> ControlSpec(-1, 1, default: 0),
			\detune -> ControlSpec(-20, 20, default: 0, units: " Hz"),
			\rate -> ControlSpec(0.125, 8, \exp, 0, 1),
			\beats -> ControlSpec(0, 20, units: " Hz"),
			
			\delay -> ControlSpec(0.0001, 1, \exp, 0, 0.3, units: " secs")
		]);
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
	var a, b, grow, curve;
	*new { arg spec, curve = -2;
		// prevent math blow up
		if (abs(curve) < 0.001, { ^LinearWarp(spec) });
		
		^super.new(spec.asSpec).init(curve);
	}
	init { arg argCurve;
		curve = argCurve;
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
		^value.squared
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^value.sqrt
	}
}

DbFaderWarp : Warp {
	//  useful mapping for amplitude faders
	map { arg value;
		// maps a value from [0..1] to spec range
		^value.squared.ampdb
	}
	unmap { arg value;
		// maps a value from spec range to [0..1]
		^value.dbamp.sqrt
	}
}

