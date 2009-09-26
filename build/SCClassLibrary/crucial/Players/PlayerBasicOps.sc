

PlayerUnop : AbstractPlayer {

	var <>operator, <>a;

	*new { arg operator, a;
		^super.new.a_(loadDocument(a)).operator_(operator)
	}
	asSynthDef {
		^thisMethod.notYetImplemented
		//^a.value.perform(operator,b.value)
	}
	/*
	asSynthDef {
		if(a.rate == \audio,{
			^SynthDef(this.defName,{ arg abus=0,out=0;
				Out.ar(out,
					In.ar(abus,a.numChannels).perform(operator)
				)
			})
		},{
			^SynthDef(this.defName,{ arg abus,out=0;
				Out.kr(out,
					In.kr(abus,a.numChannels).perform(operator)
				)
			})
		})
		//synthDefArgs { ^[ a.synthArg, patchOut.synthArg ] }
	}*/
	defName { ^(this.class.name.asString ++ operator.asString
					++ a.numChannels.asString ++ a.rate.asString) }
	children { ^[a] }
	rate { ^a.rate }
	storeArgs { ^[operator,a] }


	guiClass { ^PlayerUnopGui }

}

PlayerBinop : AbstractPlayer{
	var <>operator, <>a, <>b;
	*new { arg operator, a, b;
		a = a.loadDocument;
		b = b.loadDocument;
		if(operator == '*',{
			if(a.isNumber,{ ^PlayerAmp(b,a) });
			if(b.isNumber,{ ^PlayerAmp(a,b) });
		});
		^super.new.a_(a).b_(b).operator_(operator)
	}
	asSynthDef {
		^this.notYetImplemented(thisMethod).die
		//^a.value.perform(operator,b.value)
		/*^SynthDef(this.defName,{ arg a,b;
			a.perform(operator,b)
		})*/
	}
	storeArgs { ^[operator,a,b] }

	guiClass { ^PlayerBinopGui }
}

