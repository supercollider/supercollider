///////////  query whether some UGen can possibly free its synth from within


+ Object {
	canFreeSynth {
		^false
	}
}

+SequenceableCollection {
	canFreeSynth {
		^this.any({ arg item; item.canFreeSynth })
	}
}


+ EnvGen {
	canFreeSynth {
		^(inputs.at(4).isNumber.not or: {inputs.at(4) > 1}) and: //doneAction
			{ // it can happen that the gate is 1 and the envelope runs out
				inputs.at(0).isNumber.not or: //variable gate
				{ inputs.at(7) == -99 }		//or self releasing envelope
			}

	}
}


+ Linen {
	canFreeSynth {
		^(inputs.at(4).isNumber.not or: { inputs.at(4) > 1 }) and: { inputs.at(0).isNumber.not }
	}
}
+ Line {
	canFreeSynth { ^inputs.at(3).isNumber.not or: { inputs.at(3) > 1 } }
}
+ XLine {
	canFreeSynth { ^inputs.at(3).isNumber.not or: { inputs.at(3) > 1 }  }
}

+ Free {
	canFreeSynth { ^true }
}

+ FreeSelf {
	canFreeSynth { ^true }
}

+ DetectSilence {
	canFreeSynth { ^inputs.at(3).isNumber.not or: { inputs.at(3) > 1 }  }
}

+ Duty {
	canFreeSynth { ^inputs.at(2).isNumber.not or: { inputs.at(2) > 1 }  }
}

+ DemandEnvGen {
	canFreeSynth { ^inputs.at(9).isNumber.not or: { inputs.at(9) > 1 }  }
}

+ SynthDef {
	canFreeSynth { ^children.canFreeSynth }
	canReleaseSynth { ^this.hasGateControl and: { this.canFreeSynth } }
	hasGateControl {
		if(allControlNames.isNil) { ^false };
		^allControlNames.any { arg cn;
			cn.notNil and: { cn.name == 'gate' } and: { cn.rate !== 'scalar' }
		};
	}
}
