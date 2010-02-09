

ModalFreq : AbstractPlayerProxy {

	// players or floats will work
	var <>degree,<>scale,<>root,<>octave=4.0,<>stepsPerOctave=12.0;

	*new { arg degree=0,scale,root=0.0,octave=4.0,stepsPerOctave=12.0;
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






