Interval : Collection {	var <>start, <>end, <>step;		*new { arg start, end, step=1;		^super.newCopyArgs(start, end, step)	}		size { ^end - start div: step + 1 }	at { arg index; 		var val;		if (index < 0 or: { index >= this.size }, { ^nil });		^step * index + start;	}	do { arg function;		forBy(start, end, step, function);	}		add { ^this.shouldNotImplement }	put { ^this.shouldNotImplement }		storeParamsOn { arg stream;		stream.putAll("( ");		[start, end, step].storeItemsOn(stream);		stream.putAll(" )");	}}

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
	
	add { ^this.shouldNotImplement }
	put { ^this.shouldNotImplement }
	
	split { arg num;
		// assert: size > num
		var newRange;
		newRange = Range(start, num);
		start = start + num;
		size = size - num;
		^newRange
	}
	
	storeParamsOn { arg stream;
		stream.putAll("( ");
		[start, size].storeItemsOn(stream);
		stream.putAll(" )");
	}
}