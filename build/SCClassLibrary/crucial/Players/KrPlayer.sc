

KrPlayer : AbstractPlayer {

	var <>spec;
	
	rate { ^\control }

	kr {  ^this.subclassResponsibility(thisMethod) }
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }

	//guiClass { ^KrPlayerGui }
	
	asSynthDef { 
		// but its probably more complicated for your subclass
		^SynthDef(this.defName,{ arg outIndex = 0;
			Out.kr(outIndex,this.kr)
		})
	}
}


