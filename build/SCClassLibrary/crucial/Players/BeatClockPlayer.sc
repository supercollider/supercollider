BeatClockPlayer : KrPlayer { 		var <>tempoFactor,<>tempoBase,tempoBus;	var <>mul;
		*new { arg tempoFactor=2.0,mul=1.0,tempoBase;
		^super.new.tempoFactor_(tempoFactor).tempoBase_(tempoBase ? Tempo.default).mul_(mul)	}	
	prepareForPlay { arg group,bundle;
		//TODO: share by tempoFactor per server
		// place in a high level group

		if(patchOut.isNil,{
			super.prepareForPlay(group,bundle);
			tempoBus = TempoBus(group.asGroup.server,tempoBase);
			tempoBus.prepareForPlay(group,bundle);
		});
	}
	
	asSynthDef { 
		//unique by tempoFactor
		^SynthDef(this.defName,{ arg tempoIndex=0,outIndex = 0;
			Out.kr(outIndex,Impulse.kr(In.kr(tempoIndex) * (tempoFactor * 0.25),mul: mul))
		})
	}
	defName {
		^("BeatClockPlayer" ++ tempoFactor.asString)
	}	synthDefArgs { // synthinputs collect synthArg
		^[ tempoBus.index,patchOut.synthArg]
	}

	storeParamsOn { arg stream;		stream.storeArgs([tempoFactor])	}	guiClass { ^BeatClockPlayerGui }}