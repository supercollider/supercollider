
BeatClockPlayer : KrPlayer { 
	
	var <>tempoFactor,<>tempoBase,tempoBus;
	var <>mul;
	
	*new { arg tempoFactor=2.0,mul=1.0,tempoBase;
		^super.new.tempoFactor_(tempoFactor).tempoBase_(tempoBase ? Tempo.default).mul_(mul)
	}
	storeArgs { ^[tempoFactor,mul] } // tempoBase won't save really
	
	prepareToBundle { arg group,bundle;
		// TODO: share by tempoFactor per server
		// place in a high level group
		if(patchOut.isNil,{
			super.prepareToBundle(group,bundle);
			tempoBus = TempoBus(group.asGroup.server,tempoBase);
			tempoBus.prepareToBundle(group,bundle);
		});
	}
	
	asSynthDef { 
		//unique by tempoFactor
		^SynthDef(this.defName,{ arg i_tempoIndex=0,out = 0;
			Out.kr(out,Impulse.kr(In.kr(i_tempoIndex) * (tempoFactor.value * 0.25),mul: mul.value))
		})
	}
	defName {
		^("BeatClockPlayer" ++ tempoFactor.asString)
	}
	synthDefArgs { // synthinputs collect synthArg
		^[ \i_tempoIndex, tempoBus.index,\out,patchOut.synthArg]
	}

	guiClass { ^BeatClockPlayerGui }

}
