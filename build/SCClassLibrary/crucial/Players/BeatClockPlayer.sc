BeatClockPlayer : KrPlayer { 		var <>tempoFactor,<>tempoBase,tempoBus;		*new { arg tempoFactor=2.0,tempoBase;
		//TODO: share by tempoFactor
		^super.new.tempoFactor_(tempoFactor).tempoBase_(tempoBase ? Tempo.default)	}	
	prepareForPlay { arg group,bundle;
		if(patchOut.isNil,{
			super.prepareForPlay(group,bundle);
			tempoBus = TempoBus(group.asGroup.server,tempoBase);
			tempoBus.prepareForPlay(group,bundle);
		});
	}
	
	asSynthDef { 
		//unique by tempoFactor
		^SynthDef(this.defName,{ arg tempoIndex=0,outIndex = 0;
			Out.kr(outIndex,Impulse.kr(In.kr(tempoIndex) * (tempoFactor * 0.25)))
		})
	}
	defName {
		^("BeatClockPlayer" ++ tempoFactor.asString)
	}	synthDefArgs { // synthinputs collect synthArg
		^[ tempoBus.index,patchOut.synthArg]//.insp("Beatclok synthdefargs")
	}
	//synthArg { ^patchOut.synthArg }

	storeParamsOn { arg stream;		stream.storeArgs([tempoFactor])	}	guiClass { ^BeatClockPlayerGui }}