TChoose {
	*ar { arg trig, array;
		^Select.ar(TIRand.kr(0, array.lastIndex, trig), array)
	
	}
	*kr { arg trig, array;
		^Select.kr(TIRand.kr(0, array.lastIndex, trig), array)
	
	}
}


TWChoose : UGen {
	*ar { arg trig, array, weights, normalize=0;
		^Select.ar(TWindex.ar(trig, weights, normalize), array)
	}
	*kr { arg trig, array, weights, normalize=0;
		^Select.kr(TWindex.kr(trig, weights, normalize), array)
	}

}

/*
Channel  {

	*new { arg rate, input, offset, numChannels, wrap=true;
		var which, size, iwhich;
		size = input.size;
		
		which = Array.fill(numChannels, { arg i;
					var j;
					j = offset + i;
					if(wrap) { j % size } { j.min(size - 1) }
		});
		
		^if(offset.isKindOf(UGen)) {
			iwhich = which.floor;
			XFade2.ar(
				Select.ar(iwhich, input),
				Select.ar(iwhich + 1, input),
				which - iwhich
			)
		} {
			which.collect { arg i; input[i] }
		}
		
	}

	*ar { arg input, offset, numChannels, wrap=true;
		^this.new('audio', input, offset, numChannels, wrap)
	}
	*kr { arg input, offset, numChannels, wrap=true;
		^this.new('control', input, offset, numChannels, wrap)
	}
}

*/
