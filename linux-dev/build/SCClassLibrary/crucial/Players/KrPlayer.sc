

KrPlayer : AbstractPlayer {

	var <>spec;
	
	rate { ^\control }

	kr {  ^this.subclassResponsibility(thisMethod) }
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }

	//guiClass { ^KrPlayerGui }
	
}
