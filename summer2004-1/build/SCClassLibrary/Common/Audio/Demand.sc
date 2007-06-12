Demand : MultiOutUGen {
	*ar { arg trig, reset, demandUGens;
		^this.multiNewList(['audio', trig, reset] ++ demandUGens.asArray)
	}
	*kr { arg trig, reset, demandUGens;
		^this.multiNewList(['control', trig, reset] ++ demandUGens.asArray)
	}
	init { arg ... argInputs;
		inputs = argInputs;
		^this.initOutputs(inputs.size - 2, rate)
	}
 	checkInputs { ^this.checkSameRateAsFirstInput }
}

Dseries : UGen {
	*new { arg start = 0, step = 1, length=100;
		^this.multiNew('demand', start, step, length)
	}	
}

Dgeom : UGen {
	*new { arg start = 1, grow = 2, length=100;
		^this.multiNew('demand', start, grow, length)
	}	
}


ListDUGen : UGen {
	*new { arg list, repeats=1;
		^this.multiNewList(['demand', repeats] ++ list)
	}
}

Dseq : ListDUGen {}
Dser : ListDUGen {}
Drand : ListDUGen {}
Dxrand : ListDUGen {}

Dswitch1 : UGen {
	*new { arg list, index;
		^this.multiNewList(['demand', index] ++ list)
	}
}


Dwhite : UGen {
	*new { arg lo, hi, length=inf;
		^this.multiNew('demand', lo, hi, length)
	}
}

Diwhite : Dwhite {}

Dbrown : UGen {
	*new { arg lo, hi, step, length=inf;
		^this.multiNew('demand', lo, hi, step, length)
	}
}

Dibrown : Dbrown {}


