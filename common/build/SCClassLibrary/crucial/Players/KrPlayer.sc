

KrPlayer : AbstractPlayer {

	var <>spec;

	rate { ^\control }

	instrArgFromControl { arg control;
		^In.kr(control,this.numChannels)
	}

	//kr {  ^this.subclassResponsibility(thisMethod) }
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }
	poll { ^this }

	//guiClass { ^KrPlayerGui }

}
