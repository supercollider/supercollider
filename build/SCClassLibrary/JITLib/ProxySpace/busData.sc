+AbstractPlayer {

	busData {
		^[this.rate, this.numChannels]
	}
	
}

+Object {

	busData {
		var array, rate;
		array = this.valueForRate.asArray;
		^[array.rate, array.size];
	}
	//need to avoid that UGens add themselves to buildSynthDef
	valueForRate { arg ... args;
		var buildSynthDef, res;
		buildSynthDef = UGen.buildSynthDef;
		UGen.buildSynthDef = nil; 
		res = this.valueArray(args);
		UGen.buildSynthDef = buildSynthDef;
		^res
	}
	
}

+NodeProxy {

	busData {
		^[this.rate, this.numChannels]
	}
	
}



