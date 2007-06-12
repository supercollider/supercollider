

ModalFreq : AbstractPlayerProxy {

	// players or floats will work
	var <>degree,<>scale,<>root,<>octave=5.0,<>stepsPerOctave=12.0;
	
	*new { arg degree=0,scale,root=0.0,octave=5.0,stepsPerOctave=12.0;
		^super.new.degree_(degree).scale_(scale ?? {#[0, 2, 4, 5, 7, 9, 11]})
			.root_(root).octave_(octave).stepsPerOctave_(stepsPerOctave)
			.init
	}
	storeArgs { ^[degree,scale,root,octave,stepsPerOctave] }
	init {
		// make Patch do all the work
		source = 
		  Patch(
			UGenInstr(ModalFreqUGen,\kr),
			[degree,
			ArrayBuffer( scale  ),
			root,
			octave,
			stepsPerOctave]
		  )
	}
	
	asStream {
		var degstream,octaves,roots;
		degstream = degree.asStream;
		// take each param as stream
		octaves = octave.asStream;
		roots = root.asStream;
		
		if(stepsPerOctave != 12.0,{
			^FuncStream({
				(degstream.next.degreeToKey(scale,stepsPerOctave)
					+ (octaves.next * stepsPerOctave	) + roots.next
					* 12.0 / stepsPerOctave
					).midicps
			})
		},{
			^FuncStream({
				(degstream.next.degreeToKey(scale,stepsPerOctave)
					+ (octaves.next * stepsPerOctave	) + roots.next
					).midicps
			})
		})
	}
	guiClass { ^ModalFreqGui }
}


Midi2Freq : KrPlayer {
	// players or floats will work
	*new { arg note,octave=5.0;
		^Patch(UGenInstr(Midi2FreqUGen,\kr),[ note,octave ])
	}
	//storeArgs { ^[degree,octave] }
	
	//TODO
	//asStream
	//guiClass { ^Midi2FreqGui }
}




/*


ModalFreq : KrPlayer {

	var <>degree,<>scale,<>root,<>octave=5.0,<>stepsPerOctave=12.0;
	
	*new { arg degree=0,scale,root=0.0,octave=5.0,stepsPerOctave=12.0;
		^super.new.degree_(degree).scale_(scale ?? {FloatArray[0,1,2,3,4,5,6,7,8,9,10,11]})
			.root_(root).octave_(octave).stepsPerOctave_(stepsPerOctave);
	}
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }
	kr {
		var note,midinote;
		var steps,octs,rootval,dg,sv;
		
		// if responds to .kr ?
		steps = stepsPerOctave.value;
		octs = octave.value;
		rootval = root.value;
		dg = degree.value;
		sv = scale.value.copy;
		
		note=DegreeToKey.kr(sv,
					dg.round(1.0),
					steps);
	
		midinote = (note + (octs * steps) );
		if(rootval != 0.0, { midinote = midinote + rootval; });
	
		if(steps != 12.0,{ midinote = midinote * 12.0 / steps });
		
		^midinote.midicps;		
	}			
	
	asStream {
		var degstream;
		degstream = degree.asStream;
		// take each param as stream
		
		^FuncStream({ // not optimized
			(degstream.next.degreeToKey(scale,stepsPerOctave)
				+ (octave * stepsPerOctave	) + root
				* 12.0 / stepsPerOctave
				).midicps
		})
	}
	storeArgs { ^[degree,scale,root,octave,stepsPerOctave],1)
	}
		
	*guiClass { ^ModalFreqGui }
}


Midi2Freq : KrPlayer {

	var <>note,<>octave=5.0;
	
	*new { arg note,octave=5.0;
		^super.new.note_(note).octave_(octave);
	}
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }
	kr {
		^(note.value + (octave.value * 12.0) ).midicps;
	}			
	
	asStream {
		var notes,octaves;
		notes=note.asStream;
		octaves=octave.asStream;	
		^FuncStream({ // not optimized
			notes.next + (octaves.next * 12.0).midicps
		});
	}
	storeArgs { ^[note,octave]}
		
	*guiClass { ^Midi2FreqGui }
}

*/

