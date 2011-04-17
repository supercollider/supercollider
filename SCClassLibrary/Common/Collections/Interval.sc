Interval : Collection {
	var <>start, <>end, <>step;

	*new { arg start, end, step=1;
		^super.newCopyArgs(start, end, step)
	}

	size { ^end - start div: step + 1 }
	at { arg index;
		if (index < 0 or: { index >= this.size }, { ^nil });
		^step * index + start;
	}
	wrapAt { arg index; ^step * (index % this.size) + start }
	clipAt { arg index;
		if (index < 0) { ^start };
		if (index >= this.size) { ^end };
		^step * index + start;
	}
	do { arg function;
		forBy(start, end, step, function);
	}

	add { ^this.shouldNotImplement(thisMethod) }
	put { ^this.shouldNotImplement(thisMethod) }
	storeArgs { ^[start, end, step] }
	storeOn { arg stream;
		stream << this.class.name;
		this.storeParamsOn(stream);
	}
	printOn { arg stream; this.storeOn(stream) }
}

Range : Collection {
	var <>start, <>size;

	*new { arg start, size;
		^super.newCopyArgs(start, size);
	}
	end { ^start + size }
	do { arg function;
		for(start, start+size-1, function);
	}

	at { arg index;
		var val;
		if (index < 0 or: { index >= size }, { ^nil });
		^start + index;
	}
	includes { arg val;
		^(val >= start) and: { (val < this.end)  and: { val.frac == 0 }}
	}

	add { ^this.shouldNotImplement(thisMethod) }
	put { ^this.shouldNotImplement(thisMethod) }

	split { arg num;
		// assert: size > num
		var newRange = this.class.new(start, num);
		start = start + num;
		size = size - num;
		^newRange
	}
	storeArgs { ^[start, size] }
		storeOn { arg stream;
		stream << this.class.name;
		this.storeParamsOn(stream);
	}
	printOn { arg stream; this.storeOn(stream) }
}
