TChoose {
	*kr { arg trig, array;
		^Select.kr(TRand.kr(0, array.size, trig), array)
	
	}
}


TWChoose : UGen {
	*ar { arg trig, array, weights, normalize=0;
		^Select.ar(TWindex.ar(trig, weights, normalize), array)
	}
	*kr { arg trig, array, weights, normalize=0;
		^Select.kr(TWindex.kr(trig, weights, normalize), array)
	}

}