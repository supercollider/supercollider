GraphBuilder {
	//used to create an out ugen automatically and a fade envelope

	*wrapOut { arg name, func, rates, prependArgs, outClass=\Out, fadeTime;
		^SynthDef.new(name, { arg i_out=0;
			var result, rate, env;
			result = SynthDef.wrap(func, rates, prependArgs).asUGenInput;
			rate = result.rate;
			if(rate.isNil or: { rate === \scalar }) {
				// Out, SendTrig, [ ] etc. probably a 0.0
				result
			} {
				if(fadeTime.notNil and: { UGen.buildSynthDef.canReleaseSynth.not }) {
					result = this.makeFadeEnv(fadeTime) * result;
				};
				outClass = outClass.asClass;
				outClass.replaceZeroesWithSilence(result.asArray);
				outClass.multiNewList([rate, i_out]++result)
			}
		})
	}

	*makeFadeEnv { arg fadeTime = (0.02);
		var dt = NamedControl.kr(\fadeTime, fadeTime);
		var gate = NamedControl.kr(\gate, 1.0);
		var startVal = (dt <= 0);


		^EnvGen.kr(Env.new([startVal, 1, 0], #[1, 1], \lin, 1), gate, 1.0, 0.0, dt, 2)

	}



}


EnvGate {

	*new { arg i_level=1, gate, fadeTime, doneAction=2, curve='sin';
		var synthGate = gate ?? { NamedControl.kr(\gate, 1.0) };
		var synthFadeTime = fadeTime ?? { NamedControl.kr(\fadeTime, 0.02) };
		^EnvGen.kr(
			Env.new([ i_level, 1.0, 0.0], #[1.0, 1.0], curve, 1),
			synthGate, 1.0, 0.0, synthFadeTime, doneAction
		)
	}

}


NamedControl {
	classvar currentControls, buildSynthDef;

	var <name, <values, <lags, <rate, <fixedLag;
	var <control;

	*ar { arg  name, values, lags, spec;
		^this.new(name, values, \audio, lags, false, spec)
	}

	*kr { arg  name, values, lags, fixedLag = false, spec;
		^this.new(name, values, \control, lags, fixedLag, spec)
	}

	*ir { arg  name, values, lags, spec;
		^this.new(name, values, \scalar, lags, false, spec)
	}

	*tr { arg  name, values, lags, spec;
		^this.new(name, values, \trigger, lags, false, spec)
	}

	*new { arg name, values, rate, lags, fixedLag = false, spec;
		var res;

		this.initDict;

		name = name.asSymbol;

		if (spec.notNil) {
			spec = spec.asSpec;

			if (values.isNil) {
				values = spec.default;
			};
		};

		res = currentControls.at(name);

		lags = lags.deepCollect(inf, {|elem|
			if (elem == 0) { nil } { elem }
		});

		if (lags.rate == \scalar) {
			fixedLag = true;
		};

		if(res.isNil) {
			values = (values ? 0.0).asArray;
			res = super.newCopyArgs(name, values, lags, rate, fixedLag).init;
			currentControls.put(name, res);
		} {
			values = (values ? res.values).asArray;
			if(res.values != values) {
				Error("NamedControl: cannot have more than one set of "
					"default values in the same control.").throw;
			};
			if(rate.notNil and: { res.rate != rate }) {
				Error("NamedControl: cannot have  more than one set of "
					"rates in the same control.").throw;
			};

		};

		if(res.fixedLag and: lags.notNil) {
			if( res.lags != lags ) {
				Error("NamedControl: cannot have more than one set of "
					"fixed lag values in the same control.").throw;
			} {
				^res.control;
			}
		};

		if(spec.notNil) {
			res.spec = spec; // Set after we've finished without error.
		};

		^if(lags.notNil) {
			res.control.lag(lags).unbubble
		} {
			res.control
		}
	}

	init {
		var prefix, str;

		name !? {
			str = name.asString;
			if(str[1] == $_) { prefix = str[0] };
		};

		if(fixedLag && lags.notNil && prefix.isNil) {
			buildSynthDef.addKr(name, values.unbubble);
			if(rate === \audio) {
				control = LagControl.ar(values.flat.unbubble, lags)
			} {
				control = LagControl.kr(values.flat.unbubble, lags)
			};
		} {
			if(prefix == $a or: {rate === \audio}) {
				buildSynthDef.addAr(name, values.unbubble);
				control = AudioControl.ar(values.flat.unbubble);

			} {
				if(prefix == $t or: {rate === \trigger}) {
					buildSynthDef.addTr(name, values.unbubble);
					control = TrigControl.kr(values.flat.unbubble);
				} {
					if(prefix == $i or: {rate === \scalar}) {
						buildSynthDef.addIr(name, values.unbubble);
						control = Control.ir(values.flat.unbubble);
					} {
						buildSynthDef.addKr(name, values.unbubble);
						control = Control.kr(values.flat.unbubble);
					}
				}
			};
		};

		control = control.asArray.reshapeLike(values).unbubble;
	}

	*initDict {
		if(UGen.buildSynthDef !== buildSynthDef or: currentControls.isNil) {
			buildSynthDef = UGen.buildSynthDef;
			currentControls = IdentityDictionary.new;
		};
	}

	spec {
		^UGen.buildSynthDef.specs[name]
	}

	spec_{
		|spec|
		spec = spec.asSpec;
		this.spec !? _.setFrom(spec) ?? {
			UGen.buildSynthDef.specs[name] = spec
		};
	}
}
