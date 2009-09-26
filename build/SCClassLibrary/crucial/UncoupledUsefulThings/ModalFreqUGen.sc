
ModalFreqUGen { // a pseudo ugen

	*kr { arg degree=0,bufferProxy,root=0.0,octave=4.0,stepsPerOctave=12.0;

		var note,midinote;

		note=DegreeToKey.kr(bufferProxy.bufnumKr,
					degree.round(1.0),
					stepsPerOctave);
		//note = Select.kr(dg,sv);

		midinote = (note + (octave * stepsPerOctave) );
		//midinote = note.madd(1.0,octave * stepsPerOctave);

		if(root != 0.0, { midinote = midinote + root; });
		if(stepsPerOctave != 12.0,{ midinote = midinote * 12.0 / stepsPerOctave });
		//midinote = midinote.madd(12.0/stepsPerOctave,root);

		^midinote.midicps;
	}
	*ar { arg degree=0,bufferProxy,root=0.0,octave=4.0,stepsPerOctave=12.0;
		var note,midinote;
		note=DegreeToKey.ar(bufferProxy.bufnumKr,
					degree.round(1.0),
					stepsPerOctave);
		midinote = note.madd(1.0,octave * stepsPerOctave);
		midinote = midinote.madd(12.0/stepsPerOctave,root);
		^midinote.midicps;
	}

}

Midi2FreqUGen {

	*kr { arg note=1,octave=5;
		(note + (octave * 12.0) ).midicps;
	}
}
