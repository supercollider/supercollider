+Function {
	hatchArray {}
}

+SequenceableCollection {
	
	hatchArray {
		var streamArray, array, size, res;
		res = FuncStream.new({ arg args;
			(size div: 2).do({ arg i;
				i = i*2+1;
				array[i] = streamArray[i].next(args)
			});
			array;
		}, {
			size = this.size;
			array = Array.newFrom(this);
			streamArray = array.collect({ arg item; item.asStream });
		});
		res.reset;
		^res
	}

}

+ Pattern {
	hatchArray { ^Pevent(this).asStream.hatchArray }
}

+ Stream {
	hatchArray {
		^this.collect({ arg item; item.hatchArray })
	}
}


+ Event {
	hatchArray {
		^this.use({ ~finish.value;  this.hatchAt(\argNames) })
	}

}


