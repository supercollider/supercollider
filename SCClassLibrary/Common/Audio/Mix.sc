Mix {
	*new { arg array;
		var reducedArray = array.asArray.clump(4);
		var mixedArray = reducedArray.collect {|a|
			if (a.size == 4) {
				Sum4(*a)
			} {
				if (a.size == 3) {
					Sum3(*a)
				} {
					a.sum
				}
			}
        };

		if (mixedArray.size < 3) {
			^mixedArray.sum
		};

		if (mixedArray.size == 3) {
			^Sum3(*mixedArray)
		} {
			^Mix(mixedArray)
		}
	}

	// support this common idiom
	*fill { arg n, function;
		var array = Array.fill(n, function);
		^this.new(array);
	}

	// and these common idioms
	*ar { |array|
		var result = this.new(array);
		^switch(result.rate)
			{ \audio } { result }
			{ \control } { K2A.ar(result) }
			{ \scalar } { DC.ar(result) }
			{ Error("Unsupported rate % for Mix.ar".format(result.rate)).throw };
	}

	*kr { |array|
		var result;
		// 'rate' on an array returns the fastest rate
		// ('audio' takes precedence over 'control' over 'scalar')
		if(array.rate == \audio) {
			"Audio rate input(s) to Mix.kr will result in signal degradation.".warn;
			array.do { |unit|
				if(unit.rate == \audio) {
					(unit + unit.rate).postln;
					unit.dumpArgs;
				};
			};
			array = array.collect { |unit|
				if(unit.rate == \audio) { A2K.kr(unit) } { unit };
			};
		};
		result = this.new(array);
		^switch(result.rate)
			{ \control } { result }
			{ \scalar } { DC.kr(result) }
			{ Error("Unsupported rate % for Mix.kr".format(result.rate)).throw };
	}

	*arFill { |n, function|
		^this.ar(Array.fill(n, function))
	}

	*krFill { |n, function|
		^this.kr(Array.fill(n, function))
	}
}


NumChannels {

	*ar { arg input, numChannels = 2, mixdown = true;

		if(input.size > 1) { // collection
		   ^input
			.clump(input.size / numChannels)
			.collect { arg chan, i;
				if(chan.size == 1) {
					chan.at(0)
				} {
					if(mixdown) {
						Mix.new(chan)
					} {
						chan.at(0)
					}
				}
			}
		} {
			// single ugen or single item collection
			if(input.isSequenceableCollection) {
				input = input.at(0);
			};

			if(numChannels == 1) {
				^input
			} {
				^Array.fill(numChannels, input)
			}
		}
	}
}
