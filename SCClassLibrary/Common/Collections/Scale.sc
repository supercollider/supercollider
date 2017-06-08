Scale {
	var <degrees, <stepsPerOctave, <tuning, <>name, <>baseFreq;
	classvar <all;

	*new { | degrees = \ionian, stepsPerOctave, tuning, name = "Unknown Scale" |
		if(degrees.isKindOf(Symbol)) { Error("Please use Scale.at(name) instead.").throw };
		^super.new.init(degrees, stepsPerOctave, tuning, name);
	}

	init { | inDegrees, inStepsPerOctave, inTuning, inName |
		degrees = inDegrees;
		stepsPerOctave = inStepsPerOctave ? this.guessPPO(degrees);
		name = inName;
		baseFreq = 60.midicps;
		^this.tuning_(inTuning ? Tuning.default(stepsPerOctave));
	}

	*at { |key|
		^all.at(key)
	}

	*doesNotUnderstand { |selector, args|
		var scale = this.newFromKey(selector, args).deepCopy;
		^scale ?? { super.doesNotUnderstand(selector, args) };
	}

	*newFromKey { |key, tuning|
		var scale = this.at(key).deepCopy;
		scale ?? { ("Unknown scale " ++ key.asString).warn; ^nil };
		tuning !? { scale.tuning_(tuning.asTuning) };
		^scale
	}

	*chromatic {|tuning = \et12|
		var tuningObject = tuning.asTuning;
		var steps = tuningObject.size;
		^Scale.new(Array.series(steps), steps, tuningObject, name: "Chromatic % (%)".format(steps, tuningObject.name))
	}

	checkTuningForMismatch { |aTuning|
		^stepsPerOctave == aTuning.size;
	}

	tuning_ { | inTuning |
		inTuning = inTuning.asTuning;
		if(this.checkTuningForMismatch(inTuning)) {
			tuning = inTuning
		} {
			"Scale steps per octave % does not match tuning size ".format(stepsPerOctave).warn;
		}
	}

	guessPPO {
		// most common flavors of ET
		// pick the smallest one that contains all scale degrees
		var etTypes = #[12, 19, 24, 53, 128];
		^etTypes[etTypes.indexInBetween(degrees.maxItem).ceil];
	}

	as { |class|
		^this.semitones.as(class)
	}

	size {
		^degrees.size
	}

	semitones {
		^degrees.collect(tuning.at(_));
	}

	cents {
		^this.semitones * 100
	}

	ratios {
		^this.semitones.midiratio
	}

	at { |index|
		^tuning.at(degrees.wrapAt(index))
	}

	wrapAt { |index|
		^tuning.at(degrees.wrapAt(index))
	}

	performDegreeToKey { | scaleDegree, stepsPerOctave, accidental = 0 |
		var baseKey;
		stepsPerOctave = stepsPerOctave ? this.stepsPerOctave;
		baseKey = (stepsPerOctave * (scaleDegree div: stepsPerOctave)) + degrees.wrapAt(scaleDegree);
		^baseKey + accidental
	}

	performKeyToDegree { | degree, stepsPerOctave |
		^degrees.performKeyToDegree(degree, stepsPerOctave ? this.stepsPerOctave)
	}

	performNearestInList { | degree |
		^degrees.performNearestInList(degree)
	}

	performNearestInScale { | degree, stepsPerOctave | // collection is sorted
		^degrees.performNearestInScale(degree, stepsPerOctave ? this.stepsPerOctave)
	}

	eventMidiNote { |note, octave = 5|
		^this.tuning.midiAt(note + (octave - 5 * this.stepsPerOctave)) + baseFreq.cpsmidi
	}

	degreeToRatio { |degree, octave = 0|
		octave = octave + (degree div: degrees.size);
		^this.ratios.wrapAt(degree) * (this.octaveRatio ** octave);
	}

	degreeToFreq { |degree, rootFreq, octave|
		^this.degreeToRatio(degree, octave) * rootFreq;
	}

	*choose { |size = 7, stepsPerOctave = 12|
		var scale = this.chooseFromSelected {
			|x| (x.size == size) && (x.stepsPerOctave == stepsPerOctave)
		};
		if(scale.isNil) {
			"No known scales with size % and stepsPerOctave %".format(size, stepsPerOctave).warn
		};
		^scale
	}

	*chooseFromSelected { |selectFunc|
		selectFunc = selectFunc ? { true };
		^(all.copy.putAll(all.parent)).select(selectFunc)
		.choose.deepCopy;
	}

	*names {
		^(all.keys.asArray ++ all.parent.keys).sort
	}

	octaveRatio {
		^tuning.octaveRatio
	}

	semitonesPerOctave {
		^tuning.semitonesPerOctave
	}

	== { arg that;
		^this.compareObject(that, #[\degrees, \tuning])
	}

	hash {
		^this.instVarHash(#[\degrees, \tuning])
	}

	storeOn { |stream|
		var storedKey = this.storedKey;
		stream << this.class.name;
		if(storedKey.notNil) { stream << "." << storedKey } { this.storeParamsOn(stream) }
	}

	storedKey {
		// can be optimised later
		^all.findKeyForValue(this)
	}

	*directory {
		^this.names.collect({ |k| "\\ %: %".format(k, all.at(k).name) }).join("\n")
	}

	storeArgs { ^[degrees, stepsPerOctave, tuning, name] }


	printOn { |stream|
		this.storeOn(stream)
	}
}

Tuning {

	var <tuning, <octaveRatio, <>name;
	classvar <all;

	*new { | tuning, octaveRatio = 2.0, name = "Unknown Tuning" |
		^super.new.init(tuning, octaveRatio, name);
	}

	*at { |key|
		^all.at(key)
	}

	*doesNotUnderstand { |selector, args|
		var tuning = this.newFromKey(selector, args).deepCopy;
		^tuning ?? { super.doesNotUnderstand(selector, args) }
	}

	*newFromKey { | key |
		^all.at(key).deepCopy
	}

	*default { | stepsPerOctave |
		^this.et(stepsPerOctave);
	}

	*et { |stepsPerOctave = 12 |
		^this.new(this.calcET(stepsPerOctave), 2.0, this.etName(stepsPerOctave));
	}

	*calcET { | stepsPerOctave |
		^(0..(stepsPerOctave - 1)) * (12/stepsPerOctave)
	}

	*etName { |stepsPerOctave|
		^"ET" ++ stepsPerOctave.asString
	}

	*choose { |size = 12|
		^this.chooseFromSelected { |x| x.size == size }
	}

	*chooseFromSelected { |selectFunc|
		selectFunc = selectFunc ? { true };
		^(all.copy.putAll(all.parent)).select(selectFunc)
		.choose.deepCopy;
	}

	init { |inTuning, inOctaveRatio, inName|
		tuning = inTuning * (inOctaveRatio.ratiomidi / 12);
		octaveRatio = inOctaveRatio;
		name = inName;
		^this
	}

	ratios {
		^tuning.midiratio
	}

	semitones {
		^tuning
	}

	cents {
		^this.semitones * 100
	}

	as { |class|
		^this.semitones.as(class)
	}

	size {
		^tuning.size
	}

	at { |index|
		var pitchClassRange = tuning ++ [octaveRatio.ratiomidi];
		^pitchClassRange.blendAt(index)
	}

	midiAt { |midiNote|
		var octave = midiNote.div(this.size);
		var pitchClass = midiNote % this.size;
		var pitchClassRange = tuning ++ [octaveRatio.ratiomidi];
		^octave * octaveRatio.ratiomidi + this.at(pitchClass)
	}

	== { |argTuning|
		^this.compareObject(argTuning, #[\tuning, \octaveRatio])
	}

	hash {
		^this.instVarHash([\tuning, \octaveRatio])
	}

	semitonesPerOctave {
		^octaveRatio.log2 * 12.0
	}

	asTuning {
		^this
	}

	printOn { |stream|
		this.storeOn(stream)
	}

	storeOn { |stream|
		var storedKey = this.storedKey;
		stream << this.class.name;
		if(storedKey.notNil) { stream << "." << storedKey } { this.storeParamsOn(stream) }
	}

	storedKey {
		// can be optimised later
		^all.findKeyForValue(this)
	}

	storeArgs {
		^[tuning, octaveRatio, name]
	}

	*names {
		^(all.keys.asArray ++ all.parent.keys).sort
	}

	*directory {
		^this.names.collect({ |k| "\\ %: %".format(k, all.at(k).name) }).join("\n")
	}
}

ScaleAD : Scale {
	var <>descScale;
	*new { | degrees, stepsPerOctave, descDegrees, tuning, name = "Unknown Scale" |
		^super.new(degrees, stepsPerOctave, tuning, name)
		.descScale_(Scale(descDegrees, stepsPerOctave, tuning, name ++ "desc"))
		;
	}
	asStream { ^ScaleStream(this, 0) }
	embedInStream { ScaleStream(this).yield }

}

ScaleStream {
	var <>scale, <>curDegree;

	*new { | scale, startDegree = 0 | ^super.newCopyArgs(scale, startDegree) }

	chooseScale { |scaleDegree |
		if (scaleDegree >= curDegree) {
			curDegree = scaleDegree;
			^scale
		} {
			curDegree = scaleDegree;
			^scale.descScale
		}
	}
	performDegreeToKey { | degree, stepsPerOctave, accidental = 0 |
		^this.chooseScale(degree).performDegreeToKey(degree, stepsPerOctave, accidental);
	}


	performKeyToDegree { | degree, stepsPerOctave = 12 |
		^this.chooseScale(degree).performKeyToDegree(degree, stepsPerOctave)
	}

	performNearestInList { | degree |
		^this.chooseScale(degree).performNearestInList(degree)
	}

	performNearestInScale { | degree, stepsPerOctave=12 | // collection is sorted
		^this.chooseScale(degree).performNearestInScale(degree, stepsPerOctave)
	}
}


+ Scale {

	*initClass {

		Class.initClassTree(Tuning);

		all = IdentityDictionary[

			// TWELVE TONES PER OCTAVE
			// 5 note scales

			// https://en.wikipedia.org/wiki/Pentatonic_scale#Five_black-key_pentatonic_scales_of_the_piano
			\minorPentatonic -> Scale.new(#[0,3,5,7,10], 12, name: "Minor Pentatonic"),
			\majorPentatonic -> Scale.new(#[0,2,4,7,9], 12, name: "Major Pentatonic"),
			\ritsusen -> Scale.new(#[0,2,5,7,9], 12, name: "Ritsusen"),
			\egyptian -> Scale.new(#[0,2,5,7,10], 12, name: "Egyptian"),
			\man_gong -> Scale.new(#[0,3,5,8,10] , 12, name: "Man Gong"),

			// https://en.wikipedia.org/wiki/Japanese_mode
			\kumoijoshi -> Scale.new(#[0,2,3,7,8], 12, name: "Kumoijoshi"),

			// https://en.wikipedia.org/wiki/Hirajōshi_scale
			\hirajoshi -> Scale.new(#[0,4,6,7,11], 12, name: "Hirajoshi"),

			// https://en.wikipedia.org/wiki/Iwato_scale
			\iwato -> Scale.new(#[0,1,5,6,10], 12, name: "Iwato"),

			// TODO: Gamelan sceales and tunings: pelog, slendro, etc.
			// https://en.wikipedia.org/wiki/Pelog
			// https://en.wikipedia.org/wiki/Slendro

			// https://en.wikipedia.org/wiki/Chinese_musicology
			\gong -> Scale.new(#[0,2,4,7,9], 12, name: "Gong"),
			\shang -> Scale.new(#[0,2,5,7,10], 12, name: "Shang"),
			\jiao -> Scale.new(#[0,3,5,8,10], 12, name: "Jiao"),
			\zhi -> Scale.new(#[0,2,5,7,9], 12, name: "Zhi"),
			\yu -> Scale.new(#[0,3,5,7,10], 12, name: "Yu"),

			// 6 note scales

			// https://en.wikipedia.org/wiki/Hexatonic_scale
			\whole -> Scale.new((0,2..10), 12, name: "Whole Tone"),
			\augmented -> Scale.new(#[0,3,4,7,8,11], 12, name: "Augmented"),
			\prometheus -> Scale.new(#[0,2,4,6,9,10], 12, name: "Prometheus"),
			\blues -> Scale.new(#[0,3,5,6,7,10], name: "Blues"),
			\tritone -> Scale.new(#[0,1,4,6,7,10], 12, name: "Tritone"),
			\hexDorian -> Scale.new(#[0,2,3,5,7,10], 12, name: "Hex Dorian"),
			\hexPhrygian -> Scale.new(#[0,1,3,5,8,10], 12, name: "Hex Phrygian"),
			\hexMajor -> Scale.new(#[0,2,4,5,7,9], 12, name: "Hex Major"),
			\hexAeolian -> Scale.new(#[0,3,5,7,8,10], 12, name: "Hex Aeolian"),


			// Partch's Otonalities and Utonalities
			// from Harry Partch, Genesis of a Music,
			// see also https://en.wikipedia.org/wiki/Harry_Partch
			\partch_o1 -> Scale.new(#[0,8,14,20,25,34], 43,
				Tuning.partch, "Partch Otonality 1"),
			\partch_o2 -> Scale.new(#[0,7,13,18,27,35], 43,
				Tuning.partch, "Partch Otonality 2"),
			\partch_o3 -> Scale.new(#[0,6,12,21,29,36], 43,
				Tuning.partch, "Partch Otonality 3"),
			\partch_o4 -> Scale.new(#[0,5,15,23,30,37], 43,
				Tuning.partch, "Partch Otonality 4"),
			\partch_o5 -> Scale.new(#[0,10,18,25,31,38], 43,
				Tuning.partch, "Partch Otonality 5"),
			\partch_o6 -> Scale.new(#[0,9,16,22,28,33], 43,
				Tuning.partch, "Partch Otonality 6"),
			\partch_u1 -> Scale.new(#[0,9,18,23,29,35], 43,
				Tuning.partch, "Partch Utonality 1"),
			\partch_u2 -> Scale.new(#[0,8,16,25,30,36], 43,
				Tuning.partch, "Partch Utonality 2"),
			\partch_u3 -> Scale.new(#[0,7,14,22,31,37], 43,
				Tuning.partch, "Partch Utonality 3"),
			\partch_u4 -> Scale.new(#[0,6,13,20,28,38], 43,
				Tuning.partch, "Partch Utonality 4"),
			\partch_u5 -> Scale.new(#[0,5,12,18,25,33], 43,
				Tuning.partch, "Partch Utonality 5"),
			\partch_u6 -> Scale.new(#[0,10,15,21,27,34], 43,
				Tuning.partch, "Partch Utonality 6"),


			// 7 note scales

			// https://en.wikipedia.org/wiki/Major_scale
			\major -> Scale.new(#[0,2,4,5,7,9,11], 12, name: "Major"),
			\ionian -> Scale.new(#[0,2,4,5,7,9,11], 12, name: "Ionian"),
			\harmonicMajor -> Scale.new(#[0,2,4,5,7,8,11], 12, name: "Harmonic Major"),
			\melodicMajor -> Scale.new(#[0,2,4,5,7,8,10], 12, name: "Melodic Major"),

			// https://en.wikipedia.org/wiki/Minor_scale
			\aeolian -> Scale.new(#[0,2,3,5,7,8,10], 12, name: "Aeolian"),
			\minor -> Scale.new(#[0,2,3,5,7,8,10], 12, name: "Natural Minor"),
			\harmonicMinor -> Scale.new(#[0,2,3,5,7,8,11], 12, name: "Harmonic Minor"),
			\melodicMinor -> Scale.new(#[0,2,3,5,7,9,11], 12, name: "Melodic Minor"),
			\melodicMinorDesc -> Scale.new(#[0,2,3,5,7,8,10], 12,
				name: "Melodic Minor Descending"),

			// https://en.wikipedia.org/wiki/Dorian_mode
			\dorian -> Scale.new(#[0,2,3,5,7,9,10], 12, name: "Dorian"),

			// https://en.wikipedia.org/wiki/Phrygian_mode
			\phrygian -> Scale.new(#[0,1,3,5,7,8,10], 12, name: "Phrygian"),

			// https://en.wikipedia.org/wiki/Lydian_mode
			\lydian -> Scale.new(#[0,2,4,6,7,9,11], 12, name: "Lydian"),

			// https://en.wikipedia.org/wiki/Mixolydian_mode
			\mixolydian -> Scale.new(#[0,2,4,5,7,9,10], 12, name: "Mixolydian"),

			// https://en.wikipedia.org/wiki/Locrian_mode
			\locrian -> Scale.new(#[0,1,3,5,6,8,10], 12, name: "Locrian"),

			// https://en.wikipedia.org/wiki/Altered_scale
			\superLocrian -> Scale.new(#[0,1,3,4,6,8,10], 12, name: "Super Locrian"),
			\altered -> Scale.new(#[0,1,3,4,6,8,10], 12, name: "Altered"),

			// https://en.wikipedia.org/wiki/Ukrainian_Dorian_scale
			\romanianMinor -> Scale.new(#[0,2,3,6,7,9,10], 12, name: "Romanian Minor"),

			// https://en.wikipedia.org/wiki/Hungarian_minor_scale
			\hungarianMinor -> Scale.new(#[0,2,3,6,7,8,11], 12, name: "Hungarian Minor"),

			// https://en.wikipedia.org/wiki/Neapolitan_scale
			\neapolitanMinor -> Scale.new(#[0,1,3,5,7,8,11], 12, name: "Neapolitan Minor"),
			\neapolitanMajor -> Scale.new(#[0,1,3,5,7,9,11], 12, name: "Neapolitan Major"),

			// https://en.wikipedia.org/wiki/Enigmatic_scale
			\enigmatic -> Scale.new(#[0,1,4,6,8,10,11], 12, name: "Enigmatic"),

			// https://en.wikipedia.org/wiki/Phrygian_dominant_scale
			\phrygianDominant -> Scale.new(#[0,1,4,5,7,8,10], 12, name: "Phrygian Dominant"),

			// https://en.wikipedia.org/wiki/Major_Locrian_scale
			\leadingWhole -> Scale.new(#[0,2,4,6,8,10,11], 12, name: "Leading Whole Tone"),
			\lydianMinor -> Scale.new(#[0,2,4,6,7,8,10], 12, name: "Lydian Minor"),
			\locrianMajor -> Scale.new(#[0,2,4,5,6,8,10], 12, name: "Locrian Major"),

			// 8 note scales
			// https://en.wikipedia.org/wiki/Octatonic_scale
			\octatonicHalfWhole -> Scale.new(#[0,1,3,4,6,7,9,10], 12, name: "Octatonic Half-Whole Diminished"),
			\octatonicWholeHalf -> Scale.new(#[0,2,3,5,6,8,9,11], 12, name: "Octatonic Whole-Half Diminished"),

			// 12 note scales
			// https://en.wikipedia.org/wiki/Chromatic_scale
			\chromatic -> Scale.new((0..11), 12, name: "Chromatic"),

			// TWENTY-FOUR TONES PER OCTAVE

			// https://en.wikipedia.org/wiki/Quarter_tone
			\chromatic24 -> Scale.new((0..23), 24, name: "Chromatic 24"),

			// https://en.wikipedia.org/wiki/Arabic_maqam
			// maqam ajam
			\ajam -> Scale.new(#[0,4,8,10,14,18,22], 24, name: "Ajam"),
			\jiharkah -> Scale.new(#[0,4,8,10,14,18,21], 24, name: "Jiharkah"),
			\shawqAfza -> Scale.new(#[0,4,8,10,14,16,22], 24, name: "Shawq Afza"),

			// maqam sikah
			\sikah -> Scale.new(#[0,3,7,11,14,17,21], 24, name: "Sikah"),
			\sikahDesc -> Scale.new(#[0,3,7,11,13,17,21], 24, name: "Sikah Descending"),
			\huzam -> Scale.new(#[0,3,7,9,15,17,21], 24, name: "Huzam"),
			\iraq -> Scale.new(#[0,3,7,10,13,17,21], 24, name: "Iraq"),
			\bastanikar -> Scale.new(#[0,3,7,10,13,15,21], 24, name: "Bastanikar"),
			\mustar -> Scale.new(#[0,5,7,11,13,17,21], 24, name: "Mustar"),

			// maqam bayati
			\bayati -> Scale.new(#[0,3,6,10,14,16,20], 24, name: "Bayati"),
			\karjighar -> Scale.new(#[0,3,6,10,12,18,20], 24, name: "Karjighar"),
			\husseini -> Scale.new(#[0,3,6,10,14,17,21], 24, name: "Husseini"),

			// maqam nahawand
			\nahawand -> Scale.new(#[0,4,6,10,14,16,22], 24, name: "Nahawand"),
			\nahawandDesc -> Scale.new(#[0,4,6,10,14,16,20], 24, name: "Nahawand Descending"),
			\farahfaza -> Scale.new(#[0,4,6,10,14,16,20], 24, name: "Farahfaza"),
			\murassah -> Scale.new(#[0,4,6,10,12,18,20], 24, name: "Murassah"),
			\ushaqMashri -> Scale.new(#[0,4,6,10,14,17,21], 24, name: "Ushaq Mashri"),

			// maqam rast
			\rast -> Scale.new(#[0,4,7,10,14,18,21], 24, name: "Rast"),
			\rastDesc -> Scale.new(#[0,4,7,10,14,18,20], 24, name: "Rast Descending"),
			\suznak -> Scale.new(#[0,4,7,10,14,16,22], 24, name: "Suznak"),
			\nairuz -> Scale.new(#[0,4,7,10,14,17,20], 24, name: "Nairuz"),
			\yakah -> Scale.new(#[0,4,7,10,14,18,21], 24, name: "Yakah"),
			\yakahDesc -> Scale.new(#[0,4,7,10,14,18,20], 24, name: "Yakah Descending"),
			\mahur -> Scale.new(#[0,4,7,10,14,18,22], 24, name: "Mahur"),

			// maqam hijaz
			\hijaz -> Scale.new(#[0,2,8,10,14,17,20], 24, name: "Hijaz"),
			\hijazDesc -> Scale.new(#[0,2,8,10,14,16,20], 24, name: "Hijaz Descending"),
			\zanjaran -> Scale.new(#[0,2,8,10,14,18,20], 24, name: "Zanjaran"),

			// maqam hijazKar
			\hijazKar -> Scale.new(#[0,2,8,10,14,16,22], 24, name: "hijazKar"),

			// maqam saba
			\saba -> Scale.new(#[0,3,6,8,12,16,20], 24, name: "Saba"),
			\zamzam -> Scale.new(#[0,2,6,8,14,16,20], 24, name: "Zamzam"),

			// maqam kurd
			\kurd -> Scale.new(#[0,2,6,10,14,16,20], 24, name: "Kurd"),
			\kijazKarKurd -> Scale.new(#[0,2,8,10,14,16,22], 24, name: "Kijaz Kar Kurd"),

			// maqam nawa Athar
			\nawaAthar -> Scale.new(#[0,4,6,12,14,16,22], 24, name: "Nawa Athar"),
			\nikriz -> Scale.new(#[0,4,6,12,14,18,20], 24, name: "Nikriz"),
			\atharKurd -> Scale.new(#[0,2,6,12,14,16,22], 24, name: "Athar Kurd"),


			// Ascending/descending scales

			// https://en.wikipedia.org/wiki/Minor_scale#Melodic_minor_scale
			\melodicMinor -> ScaleAD(#[0,2,3,5,7,9,11], 12, #[0,2,3,5,7,8,10], name: "Melodic Minor"),

			// https://en.wikipedia.org/wiki/Arabic_maqam
			\sikah -> ScaleAD(#[0,3,7,11,14,17,21], 24, #[0,3,7,11,13,17,21], name: "Sikah"),
			\nahawand -> ScaleAD(#[0,4,6,10,14,16,22], 24, #[0,4,6,10,14,16,20], name: "Nahawand"),
		];

		all = all.freezeAsParent;
	}
}

+ Tuning {

	*initClass {

		all = IdentityDictionary[

			//TWELVE-TONE TUNINGS
			// https://en.wikipedia.org/wiki/Equal_temperament
			\et12 -> Tuning.new((0..11), 2, "ET12"),

			// https://en.wikipedia.org/wiki/Pythagorean_tuning
			\pythagorean -> Tuning.new([1, 256/243, 9/8, 32/27, 81/64, 4/3, 729/512, 3/2,
				128/81, 27/16, 16/9, 243/128].ratiomidi, 2, "Pythagorean"),

			// https://en.wikipedia.org/wiki/Five-limit_tuning
			\just -> Tuning.new([1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3,
				9/5, 15/8].ratiomidi, 2, "5-Limit Just Intonation"),

			// https://en.wikipedia.org/wiki/7-limit_tuning
			\sept1 -> Tuning.new([1, 16/15, 9/8, 6/5, 5/4, 4/3, 7/5, 3/2, 8/5, 5/3,
				9/5, 15/8].ratiomidi, 2, "Septimal Tritone Just Intonation"),
			\sept2 -> Tuning.new([1, 16/15, 9/8, 6/5, 5/4, 4/3, 7/5, 3/2, 8/5, 5/3,
				7/4, 15/8].ratiomidi, 2, "7-Limit Just Intonation"),

			// https://en.wikipedia.org/wiki/Meantone_temperament
			\mean4 -> Tuning.new(#[0, 0.755, 1.93, 3.105, 3.86, 5.035, 5.79, 6.965,
				7.72, 8.895, 10.07, 10.82], 2, "Meantone, 1/4 Syntonic Comma"),
			\mean5 -> Tuning.new(#[0, 0.804, 1.944, 3.084, 3.888, 5.028, 5.832, 6.972,
				7.776, 8.916, 10.056, 10.86], 2, "Meantone, 1/5 Pythagorean Comma"),
			\mean6 -> Tuning.new(#[0, 0.86, 1.96, 3.06, 3.92, 5.02, 5.88, 6.98, 7.84,
				8.94, 10.04, 10.9], 2, "Meantone, 1/6 Pythagorean Comma"),

			// https://en.wikipedia.org/wiki/Kirnberger_temperament
			\kirnberger -> Tuning.new([1, 256/243, (5.sqrt)/2, 32/27, 5/4, 4/3,
				45/32, 5 ** 0.25, 128/81, (5 ** 0.75)/2, 16/9, 15/8].ratiomidi, 2,
			"Kirnberger III"),

			// https://en.wikipedia.org/wiki/Werckmeister_temperament
			\werckmeister -> Tuning.new(#[0, 0.92, 1.93, 2.94, 3.915, 4.98, 5.9, 6.965,
				7.93, 8.895, 9.96, 10.935], 2, "Werckmeister III"),

			// https://en.wikipedia.org/wiki/Well_temperament
			\vallotti -> Tuning.new(#[0, 0.94135, 1.9609, 2.98045, 3.92180, 5.01955,
				5.9218, 6.98045, 7.9609, 8.94135, 10, 10.90225], 2, "Vallotti"),

			// https://en.wikipedia.org/wiki/Young_temperament
			\young -> Tuning.new(#[0, 0.9, 1.96, 2.94, 3.92, 4.98, 5.88, 6.98, 7.92,
				8.94, 9.96, 10.9], 2, "Young"),


			// http://www.microtonal-synthesis.com/scale_reinhard.html
			\reinhard -> Tuning.new([1, 14/13, 13/12, 16/13, 13/10, 18/13, 13/9,
				20/13, 13/8, 22/13, 13/7, 208/105].ratiomidi, 2, "Mayumi Reinhard"),

			// http://www.microtonal-synthesis.com/scale_carlos_harmonic.html
			\wcHarm -> Tuning.new([1, 17/16, 9/8, 19/16, 5/4, 21/16, 11/8, 3/2, 13/8,
				27/16, 7/4, 15/8].ratiomidi, 2, "Wendy Carlos Harmonic"),

			// http://www.microtonal-synthesis.com/scale_carlos_super_just.html
			\wcSJ -> Tuning.new([1, 17/16, 9/8, 6/5, 5/4, 4/3, 11/8, 3/2, 13/8, 5/3,
				7/4, 15/8].ratiomidi, 2, "Wendy Carlos Super Just"),

			// http://www.microtonal-synthesis.com/scale_china_lu.html
			\lu ->Tuning( [
				1, 2187/2048, 9/8, 19683/16384, 81/64, 177147/131072, 729/612, 3/2, 6561/4096,
				27/16, 59049/32768, 243/128
			].ratiomidi, 2, "Chinese Shi-er-lu scale"),

			//MORE THAN TWELVE-TONE ET
			\et19 -> Tuning.new((0 .. 18) * 12/19, 2, "ET19"),
			\et22 -> Tuning.new((0 .. 21) * 6/11, 2, "ET22"),
			\et24 -> Tuning.new((0 .. 23) * 0.5, 2, "ET24"),
			\et31 -> Tuning.new((0 .. 30) * 12/31, 2, "ET31"),
			\et41 -> Tuning.new((0 .. 40) * 12/41, 2, "ET41"),
			\et53 -> Tuning.new((0 .. 52) * 12/53, 2, "ET53"),

			//NON-TWELVE-TONE JI
			// http://www.microtonal-synthesis.com/scale_johnston_25.html
			\johnston -> Tuning.new([1, 25/24, 135/128, 16/15, 10/9, 9/8, 75/64, 6/5,
				5/4, 81/64, 32/25, 4/3, 27/20, 45/32, 36/25, 3/2, 25/16, 8/5, 5/3,
				27/16, 225/128, 16/9, 9/5, 15/8, 48/25].ratiomidi, 2, "Ben Johnston"),

			// Harry Partch, Genesis of a Music
			// see also https://en.wikipedia.org/wiki/Harry_Partch
			// and http://www.microtonal-synthesis.com/scale_partch.html
			\partch -> Tuning.new([1, 81/80, 33/32, 21/20, 16/15, 12/11, 11/10, 10/9, 9/8,
				8/7, 7/6, 32/27, 6/5, 11/9, 5/4, 14/11, 9/7, 21/16, 4/3, 27/20, 11/8,
				7/5, 10/7, 16/11, 40/27, 3/2, 32/21, 14/9, 11/7, 8/5, 18/11, 5/3, 27/16,
				12/7, 7/4, 16/9, 9/5, 20/11, 11/6, 15/8, 40/21, 64/33, 160/81].ratiomidi, 2,
			"Harry Partch"),

			// http://www.microtonal-synthesis.com/scale_catler.html
			\catler -> Tuning.new([1, 33/32, 16/15, 9/8, 8/7, 7/6, 6/5, 128/105, 16/13,
				5/4, 21/16, 4/3, 11/8, 45/32, 16/11, 3/2, 8/5, 13/8, 5/3, 27/16, 7/4,
				16/9, 24/13, 15/8].ratiomidi, 2, "Jon Catler"),

			// http://www.microtonal-synthesis.com/scale_chalmers_19.html
			\chalmers -> Tuning.new([1, 21/20, 16/15, 9/8, 7/6, 6/5, 5/4, 21/16, 4/3, 7/5,
				35/24, 3/2, 63/40, 8/5, 5/3, 7/4, 9/5, 28/15, 63/32].ratiomidi, 2,
			"John Chalmers"),

			// http://www.microtonal-synthesis.com/scale_harrison_16.html
			\harrison -> Tuning.new([1, 16/15, 10/9, 8/7, 7/6, 6/5, 5/4, 4/3, 17/12, 3/2,
				8/5, 5/3, 12/7, 7/4, 9/5, 15/8].ratiomidi, 2, "Lou Harrison"),

			// http://www.microtonal-synthesis.com/scale_indian_shruti.html
			\sruti -> Tuning.new([1, 256/243, 16/15, 10/9, 9/8, 32/27, 6/5, 5/4, 81/64,
				4/3, 27/20, 45/32, 729/512, 3/2, 128/81, 8/5, 5/3, 27/16, 16/9, 9/5,
				15/8, 243/128].ratiomidi, 2, "Sruti"),

			// ?
			\perret -> Tuning([1, 21/20, 35/32, 9/8, 7/6, 6/5, 5/4, 21/16, 4/3, 7/5, 35/24,
				3/2, 63/40, 8/5, 5/3, 7/4, 9/5, 15/8, 63/32].ratiomidi, 2, "Wilfrid Perret"),

			// http://www.michaelharrison.com
			\michael_harrison -> Tuning( [1, 28/27, 135/128, 16/15, 243/224, 9/8, 8/7, 7/6,
				32/27, 6/5, 135/112, 5/4, 81/64, 9/7, 21/16, 4/3, 112/81, 45/32, 64/45, 81/56,
				3/2, 32/21, 14/9, 128/81, 8/5, 224/135, 5/3, 27/16, 12/7, 7/4, 16/9, 15/8,
				243/128, 27/14].ratiomidi, 2, "Michael Harrison 24 tone 7-limit"),

			// HARMONIC SERIES -- length arbitary

			// https://en.wikipedia.org/wiki/Harmonic_series_(music)
			\harmonic -> Tuning.new((1 .. 24).ratiomidi, 2, "Harmonic Series 24"),

			// STRETCHED/SHRUNK OCTAVE

			// http://www.microtonal-synthesis.com/scale_bohlen_pierce.html
			\bp -> Tuning.new((0 .. 12) * (3.ratiomidi/13), 3.0, "Bohlen-Pierce"),

			// http://www.microtonal-synthesis.com/scale_carlos_alpha.html
			\wcAlpha -> Tuning.new((0 .. 14) * 0.78, (15 * 0.78).midiratio, "Wendy Carlos Alpha"),

			// http://www.microtonal-synthesis.com/scale_carlos_beta.html
			\wcBeta -> Tuning.new((0 .. 18) * 0.638, (19 * 0.638).midiratio, "Wendy Carlos Beta"),

			// http://www.microtonal-synthesis.com/scale_carlos_gamma.html
			\wcGamma -> Tuning.new((0 .. 33) * 0.351, (34 * 0.351).midiratio,
				"Wendy Carlos Gamma")
		];

		all = all.freezeAsParent;
	}

}
