
FunctionPlayer : AbstractPlayer {

	var <function,synthDef;
	var numChannels,rate; // override methods
	
	*new { arg function;
		^super.new.init(function)
	}
	init { arg f;
		function = f;
		synthDef = this.asSynthDef;
	}
	ar {	// can still be used as a building block
		^function.value
	}
	// for now, can't correctly determine due to SynthDef
	// limitations
	rate { ^'audio' }
	numChannels { ^2 }
	
	defName {
		^(this.class.name.asString ++ function.identityHash.asString)
	}
	asSynthDef { 
		^SynthDef(this.defName,{ arg outIndex = 0;
			var result;
			result = this.ar;
			if(result.isArray,{
				rate = result.first.rate;
				numChannels = result.size;
			},{
				rate = result.rate;
				numChannels = 1;
			});
			if(rate == \audio,{
				Out.ar(outIndex,result)
			},{
				Out.kr(outIndex,result)
			});
		})
	}
	//*initClass {  kill files }

}