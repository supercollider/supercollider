
PMOsc  {
	
	*ar { arg carfreq,modfreq,pmindex=0.0,modphase=0.0,mul=1.0,add=0.0; 
		^SinOsc.ar(carfreq, SinOsc.ar(modfreq, modphase, pmindex),mul,add)
	}
	
	*kr { arg carfreq,modfreq,pmindex=0.0,modphase=0.0,mul=1.0,add=0.0; 
		^SinOsc.kr(carfreq, SinOsc.ar(modfreq, modphase, pmindex),mul,add)
	}

}