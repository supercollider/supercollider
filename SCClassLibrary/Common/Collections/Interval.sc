Interval : Collection {
	var <>start, <>end, <>step;

	*new { |start, end, step = 1|
		^super.newCopyArgs(start, end, step)
	}

	size { ^end - start div: step + 1 }
	at { |index|
		if(index < 0 or: { index >= this.size }) { ^nil };
		^step * index + start
	}
	wrapAt { |index| ^step * (index % this.size) + start }
	clipAt { |index|
		if(index < 0) { ^start };
		if(index >= this.size) { ^end };
		^step * index + start
	}
	do { |function|
		forBy(start, end, step, function)
	}

	remove { ^this.shouldNotImplement(thisMethod) }
	add { ^this.shouldNotImplement(thisMethod) }
	put { ^this.shouldNotImplement(thisMethod) }
	storeArgs { ^[start, end, step] }
	storeOn { |stream|
		stream << this.class.name;
		this.storeParamsOn(stream)
	}
	printOn { |stream| this.storeOn(stream) }
}

Range : Collection {
	var <>start, <>size;

	*new { |start, size|
		^super.newCopyArgs(start, size)
	}
	end { ^start + size }
	do { |function|
		for(start, start + size - 1, function)
	}

	at { |index|
		var val;
		if(index < 0 or: { index >= size }) { ^nil };
		^start + index
	}
	includes { |val|
		^(val >= start) and: { (val < this.end) and: { val.frac == 0 } }
	}

	remove { ^this.shouldNotImplement(thisMethod) }
	add { ^this.shouldNotImplement(thisMethod) }
	put { ^this.shouldNotImplement(thisMethod) }

	split { |num|
		// assert: size > num
		var newRange = this.class.new(start, num);
		start = start + num;
		size = size - num;
		^newRange
	}
	storeArgs { ^[start, size] }
	storeOn { |stream|
		stream << this.class.name;
		this.storeParamsOn(stream)
	}
	printOn { |stream| this.storeOn(stream) }
}
