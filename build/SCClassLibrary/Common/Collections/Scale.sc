Scale {

	var <degrees, <descDegrees, <pitchesPerOctave, <tuning, <>name, lastIndex = 0, 
		setStepsNextTuning = false;
	
	*new { | degrees, tuning, descDegrees |
		// can't use arg defaults because nils are passed in by doesNotUnderstand
		// nils in tuning handled after pitchesPerOctave determined
		// nil for descDegrees is OK
		^super.new.init(degrees ? \ionian, tuning, descDegrees);
	}
	
	init { | inDegrees, inTuning, inDescDegrees |
		// Degrees may or may not set the pitchesPerOctave
		this.degrees_(inDegrees);
		// Tuning will use pitchesPerOctave if set; if not
		// will guess based on scale contents
		this.tuning_(inTuning);
		this.descDegrees_(inDescDegrees ? inDegrees);
		pitchesPerOctave = pitchesPerOctave ? tuning.size;
		^this.checkForMismatch
	}
	
	checkForMismatch {
		(pitchesPerOctave != tuning.size).if({
			(
				"Scale steps per octave " ++ pitchesPerOctave ++ 
				" does not match tuning size " ++
				tuning.size ++ ": using default tuning"
			).warn;
			tuning = Tuning.default(pitchesPerOctave);
		});
		^this
	}
	
	degrees_ { | inDegrees |
		var key;
		inDegrees.isKindOf(SequenceableCollection).if({			degrees = inDegrees.asArray;
			(degrees != degrees.asInteger).if({
				"Truncating non-integer scale degrees.".warn;
				degrees = degrees.asInteger;
			});
			name = "scale" ++ UniqueID.next.asString;
			setStepsNextTuning = true;
		}, {
			key = inDegrees ? \ionian;
			#degrees, descDegrees, pitchesPerOctave = ScaleInfo.at(key);
			name = key.asString
		})
	}
	
	descDegrees_ { | inDescDegrees |
		inDescDegrees.isKindOf(SequenceableCollection).if({
			descDegrees = inDescDegrees.asArray;
		}, {
			^descDegrees = ScaleInfo.descDegrees(inDescDegrees)
		});
	}
	
	tuning_ { | inTuning |
		var targetSteps;
		targetSteps = setStepsNextTuning.if({ this.guessSPO }, { pitchesPerOctave });
		inTuning.isKindOf(Tuning).if({
			tuning = inTuning;
		}, {
			tuning = inTuning.notNil.if({
				Tuning.new(inTuning, targetSteps);
			}, {
				Tuning.default(targetSteps);
			})
		});
		setStepsNextTuning.if({ setStepsNextTuning = false; pitchesPerOctave = tuning.size });
	}
	
	guessSPO {
		// most common flavors of ET
		// pick the smallest one that contains all scale degrees
		var etTypes = #[12, 19, 24, 53, 128];
		^etTypes[etTypes.indexInBetween(degrees.maxItem).ceil];
	}
	
	scale_ { | degrees, tuning, descDegrees |
		degrees.notNil.if({ this.degrees_(degrees) });
		tuning.notNil.if({ this.tuning_(tuning) });
		this.descDegrees_(descDegrees ? degrees);
	}
	
	asArray {
		^this.semitones
	}
	
	asADArray {
		^this.semitones(false) ++ this.semitones(true).reverse.drop(1)
	}
	
	adDegrees {
		^degrees ++ (descDegrees ? degrees).reverse.drop(1)
	}
	
	adRatios {
		^this.asADArray.midiratio
	}
	
	asFloatArray {
		var array, fa;
		array = this.asArray;
		^FloatArray.new(array.size).addAll(array);
	}
	
	size {
		^degrees.size
	}
	
	semitones { |desc = false|
		desc.if({
			^descDegrees !? descDegrees.collect({ |x| tuning.wrapAt(x) });
		},{
			this.checkForMismatch;
			^degrees.collect({ |x| tuning.wrapAt(x) });
		})
	}
	
	cents { |desc = false|
		^this.semitones * 100
	}
	
	ratios {
		^this.semitones.midiratio
	}
	
	descending {
		|index|
		^descDegrees.notNil && (index < lastIndex)
	}
	
	at { |index|
		^this.semitones(this.descending(index)).at(index) <! ( lastIndex = index )
	}
	
	wrapAt { |index|
		^this.semitones(this.descending(index)).wrapAt(index) <! ( lastIndex = index )
	}
	
	degreeToRatio { |degree, octave = 0|
		^this.ratios.at(degree) * (this.octaveRatio ** octave);
	}
	
	degreeToFreq { |degree, rootFreq, octave|
		^this.degreeToRatio(degree, octave) * rootFreq;
	}
	
	*choose { |size, tuning|
		// this is a bit pretzely, but allows steps and tuning to be constrained
		// independently, while still making sure everything matches up
		var randomScale, randomTuning, steps, selectFunc;
		randomTuning = tuning !? tuning.isKindOf(Tuning).if({ tuning }, { Tuning.new(tuning) });
		selectFunc = size.isNil.if({
			randomTuning.isNil.if({
				{ true }
			}, {
				{ |k| ScaleInfo.pitchesPerOctave(k) == randomTuning.size }
			})
		}, {
			{ |k| ScaleInfo.degrees(k).size == size }
		});
		randomScale = ScaleInfo.choose(selectFunc);
		randomTuning = randomScale.isNil.if({
			("No scales matching criteria " ++ [size, tuning].asString ++ " available.").warn;
			\et12
		}, {
			randomTuning ? Tuning.choose(ScaleInfo.pitchesPerOctave(randomScale))
		});
		^super.new.init(randomScale ? \ionian, randomTuning)
	}
	
	*doesNotUnderstand { |selector, args|
		^(ScaleInfo.includesKey(selector)).if({
			this.new(selector, args)
		}, {
			super.doesNotUnderstand(selector, args)
		})
	}
	
	doesNotUnderstand { |selector, args|
		var target;
		target = this.semitones;
		^target.respondsTo(selector).if({
			target.perform(selector, args)
		}, {
			super.doesNotUnderstand(selector, args)
		})
	}
	
	*names {
		^ScaleInfo.names
	}

	octaveRatio {
		^tuning.octaveRatio
	}
	
	stepsPerOctave {
		^tuning.stepsPerOctave
	}
}

Tuning {

	var <tuning, <>name, <octaveRatio;
	
	*new { | tuning, pitchesPerOctave, octaveRatio |
		^super.new.init(tuning, pitchesPerOctave ? 12, octaveRatio ? 2.0);
	}
	
	*default { | pitchesPerOctave |
		var defaultTuning, octaveRatio;
		defaultTuning = TuningInfo.default(pitchesPerOctave);
		octaveRatio = defaultTuning.isNil.if(2.0, { TuningInfo.octaveRatio(defaultTuning) });
		^super.new.init(defaultTuning ? this.calcDefault(pitchesPerOctave), 
			pitchesPerOctave, octaveRatio).
			name_(defaultTuning ? this.defaultName(pitchesPerOctave))
	}
	
	*et { |pitchesPerOctave|
		^super.new.init(this.calcET(pitchesPerOctave)).name_(this.etName);
	}
	
	*calcET { | pitchesPerOctave |
		^(0..(pitchesPerOctave - 1)) * (12/pitchesPerOctave)
	}
	
	*calcDefault { | pitchesPerOctave |
		^this.calcET(pitchesPerOctave)
	}
	
	*choose { |size = 12|
		^super.new.init(TuningInfo.choose(size))
	}
	
	*defaultName { |pitchesPerOctave|
		^this.etName(pitchesPerOctave)
	}
	
	*etName { |pitchesPerOctave|
		^"et" ++ pitchesPerOctave.asString
	}
	
	init { | inTuning, inpitchesPerOctave, inOctaveRatio |
		^this.tuning_(inTuning, inpitchesPerOctave, inOctaveRatio);
	}
		
	tuning_ { | inTuning, inPitchesPerOctave = 12, inOctaveRatio = 2.0 |
		var newTuning;
		inTuning.isKindOf(SequenceableCollection).if({
			tuning = inTuning.asArray;
			name = "tuning" ++ UniqueID.next.asString;
			octaveRatio = inOctaveRatio;
		}, {
			newTuning = TuningInfo.at(inTuning.asSymbol);
			newTuning.notNil.if({
				tuning = newTuning;
				name = inTuning.asString;
				octaveRatio = TuningInfo.octaveRatio(inTuning.asSymbol);
			}, {
				inTuning.notNil.if({ ("Unknown tuning: " ++ inTuning).warn });
				tuning = this.class.calcDefault(inPitchesPerOctave);
				name = this.class.defaultName(inPitchesPerOctave);
				octaveRatio = inOctaveRatio;
			})	
		});
	}

	cents_ { |cents|
		^this.tuning_(cents / 100)
	}
	
		
	ratios {
		^tuning.midiratio
	}
	
	ratioAt {
		|index|
		^this.ratios.at(index)
	}
	
	semitones {
		^tuning
	}
	
	asArray {
		^this.semitones
	}
	
	asFloatArray {
		^FloatArray.newClear(tuning.size).addAll(tuning);
	}
	
	size {
		^tuning.size;
	}
	
	*doesNotUnderstand { |selector, args|
		^(TuningInfo.includesKey(selector)).if({
			this.new(selector, args)
		}, {
			super.doesNotUnderstand(selector, args)
		})
	}
	
	doesNotUnderstand { |selector, args|
		^tuning.respondsTo(selector).if({
			tuning.perform(selector, args)
		}, {
			super.doesNotUnderstand(selector, args)
		})
	}
	
	*names {
		^TuningInfo.names
	}
	
	stepsPerOctave {
		^octaveRatio.log2 * 12.0
	}
}

ScaleInfo {

	classvar dict;
	*initClass {
	
		dict = IdentityDictionary[
			
			// TWELVE TONES PER OCTAVE
			// 5 note scales
			\minorPentatonic -> [ #[0,3,5,7,10], nil, 12 ],
			\majorPentatonic -> [ #[0,2,4,7,9], nil, 12 ],
			\ritusen -> [ #[0,2,5,7,9], nil, 12 ], // another mode of major pentatonic
			\egyptian -> [ #[0,2,5,7,10], nil, 12 ], // another mode of major pentatonic
			
			\kumoi -> [ #[0,2,3,7,9], nil, 12 ],
			\hirajoshi -> [ #[0,2,3,7,8], nil, 12 ],
			\iwato -> [ #[0,1,5,6,10], nil, 12 ], // mode of hirajoshi
			\chinese -> [ #[0,4,6,7,11], nil, 12 ], // mode of hirajoshi
			\indian -> [ #[0,4,5,7,10], nil, 12 ],
			\pelog -> [ #[0,1,3,7,8], nil, 12 ],
			
			\prometheus -> [ #[0,2,4,6,11], nil, 12 ],
			\scriabin -> [ #[0,1,4,7,9], nil, 12 ],
			
			// han chinese pentatonic scales
			\gong -> [ #[0,2,4,7,9], nil, 12 ],
			\shang -> [ #[0,2,5,7,10], nil, 12 ],
			\jiao -> [ #[0,3,5,8,10], nil, 12 ],
			\zhi -> [ #[0,2,5,7,9], nil, 12 ],
			\yu -> [ #[0,3,5,7,10], nil, 12 ],
			
			
			// 6 note scales
			\whole -> [ (0,2..10), nil, 12 ],
			\augmented -> [ #[0,3,4,7,8,11], nil, 12 ],
			\augmented2 -> [ #[0,1,4,5,8,9], nil, 12 ],
			
			// Partch's Otonalities and Utonalities
			\partch_o1 -> [ #[0,8,14,20,25,34], nil, 43 ],
			\partch_o2 -> [ #[0,7,13,18,27,35], nil, 43 ],
			\partch_o3 -> [ #[0,6,12,21,29,36], nil, 43 ],
			\partch_o4 -> [ #[0,5,15,23,30,37], nil, 43 ],
			\partch_o5 -> [ #[0,10,18,25,31,38], nil, 43 ],
			\partch_o6 -> [ #[0,9,16,22,28,33], nil, 43 ],
			\partch_u1 -> [ #[0,9,18,23,29,35], nil, 43 ],
			\partch_u2 -> [ #[0,8,16,25,30,36], nil, 43 ],
			\partch_u3 -> [ #[0,7,14,22,31,37], nil, 43 ],
			\partch_u4 -> [ #[0,6,13,20,28,38], nil, 43 ],
			\partch_u5 -> [ #[0,5,12,18,25,33], nil, 43 ],
			\partch_u6 -> [ #[0,10,15,21,27,34], nil, 43 ],
			
			// hexatonic modes with no tritone
			\hexMajor7 -> [ #[0,2,4,7,9,11], nil, 12 ],
			\hexDorian -> [ #[0,2,3,5,7,10], nil, 12 ],
			\hexPhrygian -> [ #[0,1,3,5,8,10], nil, 12 ],
			\hexSus -> [ #[0,2,5,7,9,10], nil, 12 ],
			\hexMajor6 -> [ #[0,2,4,5,7,9], nil, 12 ],
			\hexAeolian -> [ #[0,3,5,7,8,10], nil, 12 ],
			
			// 7 note scales
			\major -> [ #[0,2,4,5,7,9,11], nil, 12 ],
			\ionian -> [ #[0,2,4,5,7,9,11], nil, 12 ],
			\dorian -> [ #[0,2,3,5,7,9,10], nil, 12 ],
			\phrygian -> [ #[0,1,3,5,7,8,10], nil, 12 ],
			\lydian -> [ #[0,2,4,6,7,9,11], nil, 12 ],
			\mixolydian -> [ #[0,2,4,5,7,9,10], nil, 12 ],
			\aeolian -> [ #[0,2,3,5,7,8,10], nil, 12 ],
			\minor -> [ #[0,2,3,5,7,8,10], nil, 12 ],
			\locrian -> [ #[0,1,3,5,6,8,10], nil, 12 ],
			
			\harmonicMinor -> [ #[0,2,3,5,7,8,11], nil, 12 ],
			\harmonicMajor -> [ #[0,2,4,5,7,8,11], nil, 12 ],
			
			\melodicMinor -> [ #[0,2,3,5,7,9,11], #[0,2,3,5,7,8,10], 12 ],
			\melodicMajor -> [ #[0,2,4,5,7,8,10], nil, 12 ],
			
			\bartok -> [ #[0,2,4,5,7,8,10], nil, 12 ], // jazzers call this the hindu scale
			
			// raga modes
			\todi -> [ #[0,1,3,6,7,8,11], nil, 12 ], // maqam ahar kurd
			\purvi -> [ #[0,1,4,6,7,8,11], nil, 12 ],
			\marva -> [ #[0,1,4,6,7,9,11], nil, 12 ],
			\bhairav -> [ #[0,1,4,5,7,8,11], nil, 12 ],
			\ahirbhairav -> [ #[0,1,4,5,7,9,10], nil, 12 ],
			
			\superLocrian -> [ #[0,1,3,4,6,8,10], nil, 12 ],
			\romanianMinor -> [ #[0,2,3,6,7,9,10], nil, 12 ], // maqam nakriz
			\hungarianMinor -> [ #[0,2,3,6,7,8,11], nil, 12 ],       
			\neapolitanMinor -> [ #[0,1,3,5,7,8,11], nil, 12 ],
			\enigmatic -> [ #[0,1,4,6,8,10,11], nil, 12 ],
			\spanish -> [ #[0,1,4,5,7,8,10], nil, 12 ],
			
			// modes of whole tones with added note ->
			\leadingWhole -> [ #[0,2,4,6,8,10,11], nil, 12 ],
			\lydianMinor -> [ #[0,2,4,6,7,8,10], nil, 12 ],
			\neapolitanMajor -> [ #[0,1,3,5,7,9,11], nil, 12 ],
			\locrianMajor -> [ #[0,2,4,5,6,8,10], nil, 12 ],
			
			// 8 note scales
			\diminished -> [ #[0,1,3,4,6,7,9,10], nil, 12 ],
			\diminished2 -> [ #[0,2,3,5,6,8,9,11], nil, 12 ],
			
			// 12 note scales
			\chromatic -> [ (0..11), nil, 12 ],
			
			// TWENTY-FOUR TONES PER OCTAVE
			
			\chromatic24 -> [ (0..23), nil, 24 ],
			
			// maqam ajam
			\ajam -> [ #[0,4,8,10,14,18,22], nil, 24 ],
			\jiharkah -> [ #[0,4,8,10,14,18,21], nil, 24 ],
			\shawqAfza -> [ #[0,4,8,10,14,16,22], nil, 24 ],
			
			// maqam sikah
			\sikah -> [ #[0,3,7,11,14,17,21], #[0,3,7,11,13,17,21], 24 ],
			\huzam -> [ #[0,3,7,9,15,17,21], nil, 24 ],
			\iraq -> [ #[0,3,7,10,13,17,21], nil, 24 ],
			\bastanikar -> [ #[0,3,7,10,13,15,21], nil, 24 ],
			\mustar -> [ #[0,5,7,11,13,17,21], nil, 24 ],
			
			// maqam bayati
			\bayati -> [ #[0,3,6,10,14,16,20], nil, 24 ],
			\karjighar -> [ #[0,3,6,10,12,18,20], nil, 24 ],
			\husseini -> [ #[0,3,6,10,14,17,21], nil, 24 ],
			
			// maqam nahawand
			\nahawand -> [ #[0,4,6,10,14,16,22], #[0,4,6,10,14,16,20], 24 ],
			\farahfaza -> [ #[0,4,6,10,14,16,20], nil, 24 ],
			\murassah -> [ #[0,4,6,10,12,18,20], nil, 24 ],
			\ushaqMashri -> [ #[0,4,6,10,14,17,21], nil, 24 ],
			
			// maqam rast
			\rast -> [ #[0,4,7,10,14,18,21], #[0,4,7,10,14,18,20], 24 ],
			\suznak -> [ #[0,4,7,10,14,16,22], nil, 24 ],
			\nairuz -> [ #[0,4,7,10,14,17,20], nil, 24 ],
			\yakah -> [ #[0,4,7,10,14,18,21], #[0,4,7,10,14,18,20], 24 ],
			\mahur -> [ #[0,4,7,10,14,18,22], nil, 24 ],
			
			// maqam hijaz
			\hijaz -> [ #[0,2,8,10,14,17,20], #[0,2,8,10,14,16,20], 24 ],
			\zanjaran -> [ #[0,2,8,10,14,18,20], nil, 24 ],
			
			// maqam hijazKar
			\zanjaran -> [ #[0,2,8,10,14,16,22], nil, 24 ],
			
			// maqam saba
			\saba -> [ #[0,3,6,8,12,16,20], nil, 24 ],
			\zamzam -> [ #[0,2,6,8,14,16,20], nil, 24 ],
			
			// maqam kurd
			\kurd -> [ #[0,2,6,10,14,16,20], nil, 24 ],
			\kijazKarKurd -> [ #[0,2,8,10,14,16,22], nil, 24 ],
			
			// maqam nawa Athar
			\nawaAthar -> [ #[0,4,6,12,14,16,22], nil, 24 ],
			\nikriz -> [ #[0,4,6,12,14,18,20], nil, 24 ],
			\atharKurd -> [ #[0,2,6,12,14,16,22], nil, 24 ],
		];
	}
	
	*doesNotUnderstand { |selector, args|
		^dict.perform(selector, args)
	}
	
	*getParam {
		|name, index|
		^this.includesKey(name.asSymbol).if({ 
			dict.at(name).at(index)
		}, {
			("Unknown scale: " ++ name.asString).warn;
			nil 
		})
	}
	
	*descDegrees {
		|name|
		^this.getParam(name, 1) ? this.getParam(name, 0)
	}
	
	*degrees {
		|name|
		^this.getParam(name, 0)
	}
	
	*pitchesPerOctave {
		|name|
		^this.getParam(name, 2)
	}
	
	*choose {
		|selectFunc|
		^dict.keys.select(selectFunc ? { true }).choose;
	}
	
	*names {
		^dict.keys.asArray.sort
	}
}

TuningInfo {

	classvar dict, defaults, octaveRatios;
	
	*initClass {
		defaults = IdentityDictionary[
			43 -> \partch,
			13 -> \bp,
			19 -> \et19,
			24 -> \et24,
			53 -> \et53
		];
		
		octaveRatios = IdentityDictionary[
			\bp -> 3.0,
			\wcAlpha -> (15 * 0.78).midiratio,
			\wcBeta -> (19 * 0.638).midiratio,
			\wcGamma -> (34 * 0.351).midiratio
		];
		
		dict = IdentityDictionary[

			//TWELVE-TONE TUNINGS
			\et12 -> (0..11),

			//pythagorean
			\pythagorean -> [1, 256/243, 9/8, 32/27, 81/64, 4/3, 729/512, 3/2,
				128/81, 27/16, 16/9, 243/128].ratiomidi,
			
			//5-limit tritone
			\just -> [1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 9/5, 15/8].ratiomidi,
			
			//septimal tritone
			\sept1 -> [1, 16/15, 9/8, 6/5, 5/4, 4/3, 7/5, 3/2, 8/5, 5/3, 9/5, 15/8].ratiomidi,
			
			//septimal tritone and minor seventh
			\sept2 -> [1, 16/15, 9/8, 6/5, 5/4, 4/3, 7/5, 3/2, 8/5, 5/3, 7/4, 15/8].ratiomidi,
		
			//meantone, 1/4 syntonic comma
			\mean4 -> #[0, 0.755, 1.93, 3.105, 3.86, 5.035, 5.79, 6.965, 7.72, 8.895, 10.07, 10.82],
		
			//meantone, 1/5 Pythagorean comma
			\mean5 -> #[0, 0.804, 1.944, 3.084, 3.888, 5.028, 5.832, 6.972, 7.776, 8.916, 10.056, 10.86],
		
			//meantone, 1/6 Pythagorean comma
			\mean6 -> #[0, 0.86, 1.96, 3.06, 3.92, 5.02, 5.88, 6.98, 7.84, 8.94, 10.04, 10.9],		
			//Kirnberger III
			\kirnberger -> [1, 256/243, (5.sqrt)/2, 32/27, 5/4, 4/3, 45/32, 5 ** 0.25,
				128/81, (5 ** 0.75)/2, 16/9, 15/8].ratiomidi,
		
			//Werckmeister III
			\werckmeister -> #[0, 0.92, 1.93, 2.94, 3.915, 4.98, 5.9, 6.965, 7.93, 8.895, 9.96, 10.935],	
			//Vallotti
			\vallotti -> #[0, 0.94135, 1.9609, 2.98045, 3.92180, 5.01955, 5.9218, 6.98045,
				7.9609, 8.94135, 10, 10.90225],
				
			//Young
			\young -> #[0, 0.9, 1.96, 2.94, 3.92, 4.98, 5.88, 6.98, 7.92, 8.94, 9.96, 10.9],
				
			//Mayumi Reinhard
			\reinhard -> [1, 14/13, 13/12, 16/13, 13/10, 18/13, 13/9, 20/13, 13/8, 22/13,
				13/7, 208/105].ratiomidi,
				
			//Wendy Carlos Harmonic
			\wcHarm -> [1, 17/16, 9/8, 19/16, 5/4, 21/16, 11/8, 3/2, 13/8, 27/16, 7/4, 15/8].ratiomidi,
			
			//Wendy Carlos Super Just
			\wcSJ -> [1, 17/16, 9/8, 6/5, 5/4, 4/3, 11/8, 3/2, 13/8, 5/3, 7/4, 15/8].ratiomidi,
			
			//MORE THAN TWELVE-TONE ET
			\et19 -> ((0 .. 18) * 12/19),
			\et22 -> ((0 .. 21) * 6/11),
			\et24 -> ((0 .. 23) * 0.5),
			\et31 -> ((0 .. 30) * 12/31),
			\et41 -> ((0 .. 40) * 12/41),
			\et53 -> ((0 .. 52) * 12/53),
		
			//NON-TWELVE-TONE JI	
			//Ben Johnston
			\johnston -> [1, 25/24, 135/128, 16/15, 10/9, 9/8, 75/64, 6/5, 5/4, 81/64, 32/25, 
				4/3, 27/20, 45/32, 36/25, 3/2, 25/16, 8/5, 5/3, 27/16, 225/128, 16/9, 9/5,
				15/8, 48/25].ratiomidi,
				
			//Harry Partch
			\partch -> [1, 81/80, 33/32, 21/20, 16/15, 12/11, 11/10, 10/9, 9/8, 8/7, 7/6,
				32/27, 6/5, 11/9, 5/4, 14/11, 9/7, 21/16, 4/3, 27/20, 11/8, 7/5, 10/7, 16/11,
				40/27, 3/2, 32/21, 14/9, 11/7, 8/5, 18/11, 5/3, 27/16, 12/7, 7/4, 16/9, 9/5, 
				20/11, 11/6, 15/8, 40/21, 64/33, 160/81].ratiomidi,
				
			//Jon Catler
			\catler -> [1, 33/32, 16/15, 9/8, 8/7, 7/6, 6/5, 128/105, 16/13, 5/4, 21/16,
				4/3, 11/8, 45/32, 16/11, 3/2, 8/5, 13/8, 5/3, 27/16, 7/4, 16/9, 24/13, 15/8].ratiomidi,
				
			//John Chalmers
			\chalmers -> [1, 21/20, 16/15, 9/8, 7/6, 6/5, 5/4, 21/16, 4/3, 7/5, 35/24, 3/2,
				63/40, 8/5, 5/3, 7/4, 9/5, 28/15, 63/32].ratiomidi,
				
			//Lou Harrison
			\harrison -> [1, 16/15, 10/9, 8/7, 7/6, 6/5, 5/4, 4/3, 17/12, 3/2, 8/5, 5/3,
				12/7, 7/4, 9/5, 15/8].ratiomidi,
		
			//sruti
			\sruti -> [1, 256/243, 16/15, 10/9, 9/8, 32/27, 6/5, 5/4, 81/64, 4/3, 27/20,
				45/32, 729/512, 3/2, 128/81, 8/5, 5/3, 27/16, 16/9, 9/5, 15/8, 243/128].ratiomidi,
		
			//HARMONIC SERIES -- length arbitary
			\harmonic -> (1 .. 24).ratiomidi,
		
			//STRETCHED/SHRUNK OCTAVE
			//Bohlen-Pierce
			\bp -> ((0 .. 12) * (3.ratiomidi/13)),
			
			\wcAlpha -> ((0 .. 14) * 0.78),
			\wcBeta -> ((0 .. 18) * 0.638),
			\wcGamma -> ((0 .. 33) * 0.351)
		];
	}	

	*choose { |size|
		^dict.keys.select({ |t| dict[t].size == size }).choose;
	}

	*names {
		^dict.keys.asArray.sort
	}
	
	*default { |pitchesPerOctave|
		^defaults[pitchesPerOctave]
	}

	*doesNotUnderstand { |selector, args|
		^dict.perform(selector, args)
	}
	
	*octaveRatio { |sym|
		^octaveRatios[sym] ? 2.0
	}
}