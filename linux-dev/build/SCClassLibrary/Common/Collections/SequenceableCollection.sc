SequenceableCollection : Collection {

	// synonyms
	|@| { arg index; ^this.clipAt(index) }
	@@ { arg index; ^this.wrapAt(index) }
	@|@ { arg index; ^this.foldAt(index) }

	// fill with ramp of values
	*series { arg size, start=0, step=1;
		var i=0, obj;
		obj = this.new(size);
		size.do({ arg i;
			obj.add(start + (step * i));
		});
		^obj
	}		
	// fill with geometric series
	*geom { arg size, start, grow;
		var i=0, obj;
		obj = this.new(size);
		while ({ i < size },{
			obj.add(start);
			start = start * grow;
			i = i + 1;
		});
		^obj
	}		
	*rand { arg size, minVal, maxVal;
		var i=0, obj;
		obj = this.new(size);
		while ({ i < size },{
			obj.add(rrand(minVal, maxVal));
			i = i + 1;
		});
		^obj
	}
	*rand2 { arg size, val;
		var i=0, obj;
		obj = this.new(size);
		while ({ i < size },{
			obj.add(val.rand2);
			i = i + 1;
		});
		^obj
	}
	*linrand { arg size, minVal, maxVal;
		var i=0, obj, range;
		range = maxVal - minVal;
		obj = this.new(size);
		while ({ i < size },{
			obj.add(minVal + range.linrand);
			i = i + 1;
		});
		^obj
	}

	++ { arg aSequentialCollection; 
		var newlist;
		newlist = this.species.new(this.size + aSequentialCollection.size);
		newlist = newlist.addAll(this).addAll(aSequentialCollection);
		^newlist
	}
	
	// select an element at random
	choose { 
		^this.at(this.size.rand) 
	}
	// select an element at random using an array of weights
	wchoose { arg weights;
		^this.at(weights.windex) 
	}

	copyRange { arg start, end;
		var newColl, i;
		i = start;
		newColl = this.species.new(end - start);
		while ({ i < end },{
			newColl.add(this.at(i));
			i = i + 1;
		});
		^newColl
	}
	copyToEnd { arg start;
		^this.copyRange(start, this.size - 1)
	}
	copyFromStart { arg end;
		^this.copyRange(0, end)
	}

	indexOf { arg item;
		this.do({ arg elem, i;
			if ( item === elem, { ^i })
		});
		^nil
	}
	indexOfEqual { arg item;
		this.do({ arg elem, i;
			if ( item == elem, { ^i })
		});
		^nil
	}
	remove { arg item;
		var index;
		index = this.indexOf(item);
		^if ( index.notNil, {
			this.removeAt(index);
		},{
			nil
		});
	}
	take { arg item;
		var index;
		index = this.indexOf(item);
		^if ( index.notNil, {
			this.takeAt(index);
		},{
			nil
		});
	}
	
	// accessing
	first { if (this.size > 0, { ^this.at(0) }, { ^nil }) }
	last { if (this.size > 0, { ^this.at(this.size - 1) }, { ^nil }) }
	top { ^this.last }
	putFirst { arg obj; if (this.size > 0, { ^this.put(0, obj) }) }
	putLast { arg obj; if (this.size > 0, { ^this.put(this.size - 1, obj) }) }

	// ordering
	doAdjacentPairs { arg function;
		(this.size - 1).do({ arg i;
			function.value(this.at(i), this.at(i+1), i);
		})
	}
	separate { arg function;
		var list, sublist;
		list = Array.new;
		sublist = this.species.new;
		this.doAdjacentPairs({ arg a, b, i;
			sublist = sublist.add(a);
			if ( function.value(a, b, i), {
				list = list.add(sublist);
				sublist = this.species.new;
			});
		});
		sublist = sublist.add(this.last);
		list = list.add(sublist);
		^list
	}
	delimit { arg function;
		var list, sublist;
		list = Array.new;
		sublist = this.species.new;
		this.do({ arg item, i;
			if ( function.value(item, i), {
				list = list.add(sublist);
				sublist = this.species.new;
			},{
				sublist = sublist.add(item);
			});
		});
		list = list.add(sublist);
		^list
	}
	clump { arg groupSize;
		var list, sublist;
		list = Array.new;
		sublist = this.species.new;
		this.do({ arg item;
			sublist = sublist.add(item);
			if (sublist.size >= groupSize, {
				list = list.add(sublist);
				sublist = this.species.new;
			});
		});
		if (sublist.size > 0, { list = list.add(sublist); });
		^list
	}
	clumps { arg groupSizeList;
		var list, sublist, i=0;
		list = Array.new;
		sublist = this.species.new;
		this.do({ arg item;
			sublist = sublist.add(item);
			if (sublist.size >= groupSizeList.wrapAt(i), {
				i = i + 1;
				list = list.add(sublist);
				sublist = this.species.new;
			});
		});
		if (sublist.size > 0, { list = list.add(sublist); });
		^list
	}
	curdle { arg probability;
		^this.separate({ probability.coin });
	}
	flatten { arg numLevels=1;
		var list;
		
		if (numLevels <= 0, { ^this });
		numLevels = numLevels - 1;
		
		list = this.species.new;
		this.do({ arg item;
			if (item.respondsTo('flatten'), {
				list = list.addAll(item.flatten(numLevels));
			},{
				list = list.add(item);
			});
		});
		^list
	}
	
	flat {
		var list;	
		list = this.species.new;
		this.do({ arg item, i;
			if (item.respondsTo('flat'), {
				list = list.addAll(item.flat);
			},{
				list = list.add(item);
			});
		});
		^list
	}

	flop {
		var list, size, maxsize;
		
		size = this.size;
		maxsize = 0;
		this.do({ arg sublist;
			var sz;
			sz = if (sublist.isSequenceableCollection, { sublist.size },{ 1 });
			if (sz > maxsize, { maxsize = sz });
		});
						 
		list = this.species.fill(maxsize, { this.species.new(size) });
		this.do({ arg isublist, i;
			if (isublist.isSequenceableCollection, {
				list.do({ arg jsublist, j;
					jsublist.add( isublist.wrapAt(j); );
				});
			},{
				list.do({ arg jsublist, j;
					jsublist.add( isublist );
				});
			});
		});
		^list
	}
	unlace { arg numlists;
		var size, list, sublist;
		
		size = (this.size + numlists - 1) div: numlists;
		list = this.species.fill(numlists, { this.species.new(size) });
		this.do({ arg item, i;
			sublist = list.at(i % numlists);
			sublist.add(item);
		});
		^list
	}
		
	// pitch operations
	degreeToKey { arg scale, stepsPerOctave=12;
		var size;
		size = scale.size;
		^this.collect({ arg scaleDegree;
			scaleDegree = scaleDegree.asInteger;
			(stepsPerOctave * (scaleDegree div: size)) + scale.wrapAt(scaleDegree)
		});
	}
	transposeKey { arg amount, octave=12;
		^((this + amount) % octave).sort
	}
	mode { arg degree, octave=12;
		^(rotate(this, degree.neg) - this.wrapAt(degree)) % octave
	}

	// supports a variation of Mikael Laurson's rhythm lists.
	convertRhythm {
		var list, tie;
		list = List.new;
		tie = this.convertOneRhythm(list);
		if (tie > 0.0, { list.add(tie) });  // check for tie at end of rhythm
		^list
	}
	convertOneRhythm { arg list, tie = 0.0, stretch = 1.0;
		var beats, divisions, repeats;
		
		#beats, divisions, repeats = this;
		stretch = stretch * beats / divisions.sum;
		repeats.do({
			divisions.do({ arg val;
				if (val.isSequenceableCollection, {
					tie = val.convertOneRhythm(list, tie, stretch)
				},{
					val = val * stretch;
					if (val > 0.0, { 
						list.add(val + tie); 
						tie = 0.0; 
					},{ 
						tie = tie - val 
					});
				});
			});
		});
		^tie
	}

	// misc math
	blend { arg anotherSeqColl, blendFrac = 0.5;
		// this blends between two collections assumed to contain numbers
		// blendFrac should be from zero to one
		^this.collect({ arg item, i; item.blend(anotherSeqColl.at(i), blendFrac); });
	}
	
	isSequenceableCollection { ^true }
	
	// unary math ops
	neg { ^this.performUnaryOp('neg') }
	bitNot { ^this.performUnaryOp('bitNot') }
	abs { ^this.performUnaryOp('abs') }
	ceil { ^this.performUnaryOp('ceil') }
	floor { ^this.performUnaryOp('floor') }
	frac { ^this.performUnaryOp('frac') }
	sign { ^this.performUnaryOp('sign') }
	squared { ^this.performUnaryOp('squared') }
	cubed { ^this.performUnaryOp('cubed') }
	sqrt { ^this.performUnaryOp('sqrt') }
	exp { ^this.performUnaryOp('exp') }
	reciprocal { ^this.performUnaryOp('reciprocal') }
	midicps { ^this.performUnaryOp('midicps') }
	cpsmidi { ^this.performUnaryOp('cpsmidi') }
	midiratio { ^this.performUnaryOp('midiratio') }
	ratiomidi { ^this.performUnaryOp('ratiomidi') }
	ampdb { ^this.performUnaryOp('ampdb') }
	dbamp { ^this.performUnaryOp('dbamp') }	
	octcps { ^this.performUnaryOp('octcps') }
	cpsoct { ^this.performUnaryOp('cpsoct') }
	log { ^this.performUnaryOp('log') }
	log2 { ^this.performUnaryOp('log2') }
	log10 { ^this.performUnaryOp('log10') }
	sin { ^this.performUnaryOp('sin') }
	cos { ^this.performUnaryOp('cos') }
	tan { ^this.performUnaryOp('tan') }
	asin { ^this.performUnaryOp('asin') }
	acos { ^this.performUnaryOp('acos') }
	atan { ^this.performUnaryOp('atan') }
	sinh { ^this.performUnaryOp('sinh') }
	cosh { ^this.performUnaryOp('cosh') }
	tanh { ^this.performUnaryOp('tanh') }
	rand { ^this.performUnaryOp('rand') }
	rand2 { ^this.performUnaryOp('rand2') }
	linrand { ^this.performUnaryOp('linrand') }
	bilinrand { ^this.performUnaryOp('bilinrand') }
	sum3rand { ^this.performUnaryOp('sum3rand') }

	distort { ^this.performUnaryOp('distort') }
	softclip { ^this.performUnaryOp('softclip') }
	nyqring { ^this.performUnaryOp('nyqring') }
	coin { ^this.performUnaryOp('coin') }
	even { ^this.performUnaryOp('even') }
	odd { ^this.performUnaryOp('odd') }
	isPositive { ^this.performUnaryOp('isPositive') }
	isNegative { ^this.performUnaryOp('isNegative') }
	isStrictlyPositive { ^this.performUnaryOp('isStrictlyPositive') }

	rectWindow { ^this.performUnaryOp('rectWindow') }
	hanWindow { ^this.performUnaryOp('hanWindow') }
	welWindow { ^this.performUnaryOp('welWindow') }
	triWindow { ^this.performUnaryOp('triWindow') }

	scurve { ^this.performUnaryOp('scurve') }
	ramp { ^this.performUnaryOp('ramp') }

	asFloat { ^this.performUnaryOp('asFloat') }
	asInteger { ^this.performUnaryOp('asInteger') }
	
	real { ^this.performUnaryOp('real') }
	imag { ^this.performUnaryOp('imag') }

	magnitude { ^this.performUnaryOp('magnitude') }
	magnitudeApx { ^this.performUnaryOp('magnitudeApx') }
	phase { ^this.performUnaryOp('phase') }
	angle { ^this.performUnaryOp('angle') }

	rho { ^this.performUnaryOp('rho') }
	theta { ^this.performUnaryOp('theta') }
	
	// binary math ops
	+ { arg aNumber; ^this.performBinaryOp('+', aNumber) }
	- { arg aNumber; ^this.performBinaryOp('-', aNumber) }
	* { arg aNumber; ^this.performBinaryOp('*', aNumber) }
	/ { arg aNumber; ^this.performBinaryOp('/', aNumber) }
	div { arg aNumber; ^this.performBinaryOp('div', aNumber) }
	mod { arg aNumber; ^this.performBinaryOp('mod', aNumber) }
	pow { arg aNumber; ^this.performBinaryOp('pow', aNumber) }
	min { arg aNumber; ^this.performBinaryOp('min', aNumber) } 
	max { arg aNumber; ^this.performBinaryOp('max', aNumber) }

	
	<  { arg aNumber; ^this.performBinaryOp('<',  aNumber) }
	<= { arg aNumber; ^this.performBinaryOp('<=', aNumber) }
	>  { arg aNumber; ^this.performBinaryOp('>',  aNumber) }
	>= { arg aNumber; ^this.performBinaryOp('>=', aNumber) }

	bitAnd { arg aNumber; ^this.performBinaryOp('bitAnd', aNumber) }
	bitOr { arg aNumber; ^this.performBinaryOp('bitOr', aNumber) }
	bitXor { arg aNumber; ^this.performBinaryOp('bitXor', aNumber) }	
	lcm { arg aNumber; ^this.performBinaryOp('lcm', aNumber) }
	gcd { arg aNumber; ^this.performBinaryOp('gcd', aNumber) }
	round { arg aNumber; ^this.performBinaryOp('round', aNumber) }
	trunc { arg aNumber; ^this.performBinaryOp('trunc', aNumber) }
	atan2 { arg aNumber; ^this.performBinaryOp('atan2', aNumber) }	
	hypot { arg aNumber; ^this.performBinaryOp('hypot', aNumber) }
	hypotApx { arg aNumber; ^this.performBinaryOp('hypotApx', aNumber) }
	leftShift { arg aNumber; ^this.performBinaryOp('leftShift', aNumber) }
	rightShift { arg aNumber; ^this.performBinaryOp('rightShift', aNumber) }
	unsignedRightShift { arg aNumber; ^this.performBinaryOp('unsignedRightShift', aNumber) }
	ring1 { arg aNumber; ^this.performBinaryOp('ring1', aNumber) }
	ring2 { arg aNumber; ^this.performBinaryOp('ring2', aNumber) }
	ring3 { arg aNumber; ^this.performBinaryOp('ring3', aNumber) }
	ring4 { arg aNumber; ^this.performBinaryOp('ring4', aNumber) }
	difsqr { arg aNumber; ^this.performBinaryOp('difsqr', aNumber) }
	sumsqr { arg aNumber; ^this.performBinaryOp('sumsqr', aNumber) }
	sqrsum { arg aNumber; ^this.performBinaryOp('sqrsum', aNumber) }
	sqrdif { arg aNumber; ^this.performBinaryOp('sqrdif', aNumber) }
	absdif { arg aNumber; ^this.performBinaryOp('absdif', aNumber) }
	thresh { arg aNumber; ^this.performBinaryOp('thresh', aNumber) }
	amclip { arg aNumber; ^this.performBinaryOp('amclip', aNumber) }
	scaleneg { arg aNumber; ^this.performBinaryOp('scaleneg', aNumber) }
	clip2 { arg aNumber; ^this.performBinaryOp('clip2', aNumber) }
	fold2 { arg aNumber; ^this.performBinaryOp('fold2', aNumber) }
	wrap2 { arg aNumber; ^this.performBinaryOp('wrap2', aNumber) }
	excess { arg aNumber; ^this.performBinaryOp('excess', aNumber) }
	firstArg { arg aNumber; ^this.performBinaryOp('firstArg', aNumber) }
	rrand { arg aNumber; ^this.performBinaryOp('rrand', aNumber) }
	exprand { arg aNumber; ^this.performBinaryOp('exprand', aNumber) }

	// math op dispatch support
	performUnaryOp { arg aSelector;
		^this.collect({ arg item; item.perform(aSelector) });
	}

	performBinaryOp { arg aSelector, theOperand;
		var i, minSize, newList;
		^theOperand.performBinaryOpOnSeqColl(aSelector, this);
	}
	performBinaryOpOnSeqColl { arg aSelector, theOperand;
		var minSize, newList;
		minSize = this.size min: theOperand.size;
		newList = this.species.new(minSize);
		minSize.do({ arg i;
			newList.add(theOperand.at(i).perform(aSelector, this.at(i)));
		});
		^newList
	}
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; 
		^this.collect({ arg item; 
			aNumber.perform(aSelector, item)
		}) 
	}
	performBinaryOpOnComplex { arg aSelector, aComplex; 
		^this.collect({ arg item; 
			aComplex.perform(aSelector, item)
		}) 
	}
	performBinaryOpOnInfinitum { arg aSelector, aNumber; 
		^this.collect({ arg item; 
			aNumber.perform(aSelector, item)
		}) 
	}
	
	// support UGen clock division arguments.
	cdiv { ^this.collect({ arg item; item.cdiv; }); }
	minDiv {
		var div;
		div = UGen.scalarRate;
		this.do({ arg item; div = min(div, item.cdiv) });
		^div
	}
	maxDiv {
		var div;
		div = 1;
		this.do({ arg item; div = max(div, item.cdiv) });
		^div
	}
	
	// sorting
	sort { arg function; 
		if (function.isNil, { function = { arg a, b; a < b }; }); 
		this.sortRange(0, this.size - 1, function) 
	}
	
	swap { arg i, j;
		var temp;
		temp = this.at(i);
		this.put(i, this.at(j));
		this.put(j, temp);
	}
	
	sortRange { arg i, j, function;
		//Sort elements i through j of this to be nondescending according to
		// function.
		var di, dij, dj, tt, ij, k, l, n;
				
		// The prefix d means the data at that index.
		if ((n = j + 1  - i) <= 1, { ^this });	// Nothing to sort.
		
		//Sort di,dj.
		di = this.at(i);
		dj = this.at(j);
		if (function.value(di, dj).not, { // i.e., should di precede dj?
			this.swap(i,j);
				 tt = di;
				 di = dj;
				 dj = tt;
		});
		if ( n > 2, { // More than two elements.
			ij = (i + j) div: 2;  // ij is the midpoint of i and j.
			dij = this.at(ij);  // Sort di,dij,dj.  Make dij be their median.
			if (function.value(di,  dij), {  // i.e. should di precede dij?
				if (function.value(dij, dj).not, {  // i.e., should dij precede dj?
					this.swap(j, ij);
					dij = dj;
				})
			},{ // i.e. di should come after dij"
				this.swap(i, ij);
				dij = di;
			});
			if ( n > 3, { // More than three elements.
				// Find k>i and l<j such that dk,dij,dl are in reverse order.
				// Swap k and l.  Repeat this procedure until k and l pass each other.
				k = i;
				l = j;
				while ({
				 	while ({
				 		l = l - 1;  
				 		k <= l and: { function.value(dij, this.at(l)) }
				 	}); // i.e. while dl succeeds dij
				 	while ({
				 		k = k + 1;  
				 		k <= l and: { function.value(this.at(k), dij) };
				 	}); // i.e. while dij succeeds dk
				 	k <= l
				},{
					this.swap(k, l); 
				});
		// Now l<k (either 1 or 2 less), and di through dl are all less than or equal to dk
		// through dj.  Sort those two segments.
				this.sortRange(i, l, function);
				this.sortRange(k, j, function);
			});
		});
	}

	// streaming
	*streamContents { arg function;
		var stream;
		stream = CollStream.on(this.new(100));
		function.value(stream);
		^stream.contents
	}
	*streamContentsLimit { arg function, limit=2000;
		var stream;
		stream = LimitedWriteStream.on(this.new(100 min: limit));
		stream.limit_(limit).limitFunc_({ ^stream.contents });
		function.value(stream);
		^stream.contents
	}
	
}
