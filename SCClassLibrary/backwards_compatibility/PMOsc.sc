// these pseudo ugens replicate some SC2 ugens
// still useful as shorthands

PMOsc  {

	*ar { arg carfreq,modfreq,pmindex=0.0,modphase=0.0,mul=1.0,add=0.0;
		^SinOsc.ar(carfreq, SinOsc.ar(modfreq, modphase, pmindex),mul,add)
	}

	*kr { arg carfreq,modfreq,pmindex=0.0,modphase=0.0,mul=1.0,add=0.0;
		^SinOsc.kr(carfreq, SinOsc.kr(modfreq, modphase, pmindex),mul,add)
	}

}


MultiTap  {

	*ar { arg timesArray, levelsArray, in = 0.0, mul = 1.0, add = 0.0,bufnum;

		var sampleRate;
		timesArray = timesArray.dereference;
		levelsArray = levelsArray.dereference;
		RecordBuf.ar(in,bufnum,0.0, run: -1.0);
		sampleRate = BufSampleRate.kr(bufnum);

		^Mix.fill(timesArray.size,{ arg i;
			PlayBuf.ar(in.numChannels,
					bufnum, -1.0,1.0,
					timesArray.at(i) * sampleRate,
					loop: 1)
				.madd(levelsArray.at(i) ? 1.0)
		}).madd(mul,add);
	}
}

/*

GrainTap {

	// overlap determines density
	*ar { arg bufnum, grainDur = 0.2, pchRatio = 1.0,
			pchDispersion = 0.0, timeDispersion = 0.0, overlap = 2.0, mul = 1.0, add = 0.0;

		var sampleRate;
		sampleRate = BufSampleRate.kr(bufnum);
		pchRatio = pchRatio * BufRateScale.kr(bufnum);


		^Mix.fill( howMany ? ,{ arg i;
			PlayBuf.ar(buf.numChannels,
					bufnum,1.0,1.0,
					loop: 1)
		}).madd(mul,add);

	}
}

*/
