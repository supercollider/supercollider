+SequenceableCollection {
	rate { 
		var rate;
		rate = this.first.rate;
		if(this.size == 1, { ^rate });
		^if(this.every({ arg item; item.rate === rate }), 
			rate, nil);
	}
}

+Nil {
	rate { ^nil }
	numChannels { ^nil }
}

