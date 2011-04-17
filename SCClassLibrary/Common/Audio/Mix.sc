Mix {
	*new { arg array;
		var sum;
		array.asArray.do({ arg val, i;
			if (sum.isNil, { sum = val },{ sum = sum + val });
		});
		^sum
	}
	// support this common idiom
	*fill { arg n, function;
		var val, sum;
		n.do({ arg i;
			val = function.value(i);
			if (sum.isNil, { sum = val },{ sum = sum + val });
		});
		^sum;
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
						Mix.ar(chan)
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
