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
