GraphBuilder {
	//used to create an out ugen automatically and a fade envelope
	
	*wrapOut { arg name, func, rates, prependArgs, outClass=\Out, fadeTime;
		^SynthDef.new(name, { arg i_out=0;
			var result, rate, env;
			result = SynthDef.wrap(func, rates, prependArgs).asUGenInput;
			rate = result.rate;
			if(rate === \scalar,{
				// Out, SendTrig etc. probably a 0.0
				result
			},{
				if(fadeTime.notNil, {
						result = this.makeFadeEnv(fadeTime) * result;
				});
				outClass = outClass.asClass;
				outClass.replaceZeroesWithSilence(result.asArray);
				outClass.multiNewList([rate, i_out]++result)
			})
		})
	}

	*makeFadeEnv { arg fadeTime;
		var dt, gate;
		#dt, gate = Control.names(['fadeTime', 'gate']).kr([fadeTime, 1.0]);
		^Linen.kr(gate, dt, 1.0, dt, 2);
	
	}



}


EnvGate {
		classvar currentControl, buildSynthDef;
	
				
		*new { arg i_level=1, gate, fadeTime, doneAction=2, curve='sin';
			var synthGate, synthFadeTime, startVal;
				if(gate.isNil and: { fadeTime.isNil }) {
					#synthGate, synthFadeTime = this.currentControl
				} {
					synthGate = gate ?? { Control.names('gate').kr(1.0) };					synthFadeTime = fadeTime ?? { Control.names('fadeTime').kr(0.02) };
				};
				startVal = (synthFadeTime <= 0);
				^EnvGen.kr(
					Env.new([ startVal,1,0], #[1,1],curve,1),
					synthGate, i_level, 0.0, synthFadeTime, doneAction
				)
		}
		
		// this allows several instances within a single synthdef
		*currentControl {
			if(this.hasCurrentControl.not) {
				currentControl = Control.names(['gate', 'fadeTime']).kr([1, 0.02]);
				buildSynthDef = UGen.buildSynthDef;
			}
			^currentControl
		}
		*hasCurrentControl {
			^UGen.buildSynthDef === buildSynthDef and: { currentControl.notNil }
		}

}


NamedControl {
	classvar currentControls, buildSynthDef;
	var <name, <values, <lags, <fixedLag;
	var <control;
	
	*new { arg name, values = [0], lags, fixedLag = false;
		var res;
		values = values.asArray;
		
		this.initDict;
		res = currentControls.at(name);
		
		if(res.isNil) {
			res = super.newCopyArgs(name, values, lags, fixedLag).init;
			currentControls.put(name, res);
		} {
			if(res.values != values) { 
				Error("NamedControl: cannot have two sets of default values.").throw;
			};
		};
		
		if(res.fixedLag and: { res.lags != lags }) {
			Error("NamedControl: cannot have more than one set of fixed lag values.").throw;
		};
		^if(lags.notNil) {
			res.control.lag(lags.asArray)
		} {
			res.control
		}
	}
	
	*initDict {
		if(UGen.buildSynthDef !== buildSynthDef or: currentControls.isNil) {
			buildSynthDef = UGen.buildSynthDef;
			currentControls = IdentityDictionary.new;
		};
	}
	
	init {
		var prefix, ctlName, ctl;
		
		name !? {
			name = name.asString;
			if(name[1] == $_) { prefix = name[0]; ctlName = name[2..] } { ctlName = name };
			Control.names([ctlName.asSymbol]);
		};
		
		if(fixedLag && lags.notNil && prefix.isNil) { 
			control = LagControl.kr(values.flat, lags);
		} {
			if(prefix == $a) { 
				control = AudioControl.ar(values.flat);
			} {
				if(prefix == $t) { 
					control = TrigControl.kr(values.flat);
				} {
					control = Control.kr(values.flat);
				}
			};
		};
		
		control = control.asArray.reshapeLike(values);
	}
	

}

