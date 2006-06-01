SequenceableCollection : Collection {	
	// synonyms
	|@| { arg index; ^this.clipAt(index) }
	@@ { arg index; ^this.wrapAt(index) }
	@|@ { arg index; ^this.foldAt(index) }

	// fill with ramp of values
	*series { arg size, start=0, step=1;
		var obj = this.new(size);
		size.do {|i|
			obj.add(start + (step * i));
		};
		^obj
	}		
	// fill with geometric series
	*geom { arg size, start, grow;
		var i=0;
		var obj = this.new(size);
		while ({ i < size },{
			obj.add(start);
			start = start * grow;
			i = i + 1;
		});
		^obj
	}		
	// fill with fibonacci series
	*fib { arg size, a=0.0, b=1.0;
		var i=0, temp;
		var obj = this.new(size);
		while { i < size }{
			obj.add(b);
			temp = b;
			b = a + b;
			a = temp;
			i = i + 1;
		};
		^obj
	}		
	*rand { arg size, minVal, maxVal;
		var i=0;
		var obj = this.new(size);
		while ({ i < size },{
			obj.add(rrand(minVal, maxVal));
			i = i + 1;
		});
		^obj
	}
	*exprand { arg size, minVal, maxVal;
		var i=0;
		var obj = this.new(size);
		while ({ i < size },{
			obj.add(exprand(minVal, maxVal));
			i = i + 1;
		});
		^obj
	}
	*rand2 { arg size, val;
		var i=0;
		var obj = this.new(size);
		while ({ i < size },{
			obj.add(val.rand2);
			i = i + 1;
		});
		^obj
	}
	*linrand { arg size, minVal, maxVal;
		var i=0;
		var range = maxVal - minVal;
		var obj = this.new(size);
		while ({ i < size },{
			obj.add(minVal + range.linrand);
			i = i + 1;
		});
		^obj
	}

	++ { arg aSequenceableCollection; 
		var newlist = this.species.new(this.size + aSequenceableCollection.size);
		newlist = newlist.addAll(this).addAll(aSequenceableCollection);
		^newlist
	}
	+++ { arg aSequenceableCollection; 
		aSequenceableCollection = aSequenceableCollection.asSequenceableCollection;
		^this.collect {|item, i|
			item.asSequenceableCollection ++ aSequenceableCollection.wrapAt(i)
		}
	}
	asSequenceableCollection { ^this }
	
	// select an element at random
	choose { 
		^this.at(this.size.rand) 
	}
	// select an element at random using an array of weights
	wchoose { arg weights;
		^this.at(weights.windex) 
	}
	
	== { | aCollection |
		if (aCollection.class != this.class) { ^false };
		if (this.size != aCollection.size) { ^false };
		this.do { | item, i |
			if (item != aCollection[i]) { ^false };
		};
		^true
	}

	copyRange { arg start, end;
		var newColl;
		var i = start;
		newColl = this.species.new(end - start);
		while ({ i < end },{
			newColl.add(this.at(i));
			i = i + 1;
		});
		^newColl
	}
	keep { arg n;
		var size;
		if (n>=0) {
			^this.copyRange(0, n-1)
		}{
			size = this.size;
			^this.copyRange(size+n, size-1)
		}
	}
	drop { arg n;
		var size = this.size;
		if (n>=0) {
			^this.copyRange(n, size-1)
		}{
			^this.copyRange(0, size+n-1)
		}
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
	
	indexOfGreaterThan { arg val;
		^this.detectIndex { |item| item > val };
	}
	
	indexIn { arg val; // collection is sorted, returns closest index
		var i, a, b;
		var j = this.indexOfGreaterThan(val);
		if(j.isNil) { ^this.size - 1 };
		if(j == 0) { ^j };
		i = j - 1;
		^if((val - this[i]) < (this[j] - val)) { i } { j }
	}
	
	indexInBetween { arg val; // collection is sorted, returns linearly interpolated index
		var a, b, div;
		var i = this.indexOfGreaterThan(val);
		if(i.isNil) { ^this.size - 1 };
		if(i == 0) { ^i };
		a = this[i-1]; b = this[i];
		div = b - a;
		if(div == 0) { ^i };
		^((val - a) / div) + i - 1
	}
	
	resamp0 { arg newSize;
		var factor = this.size - 1 / (newSize - 1);
		^this.species.fill(newSize, { |i| this.at((i * factor).round(1.0).asInteger) })
	}
	
	resamp1 { arg newSize;
		var factor = this.size - 1 / (newSize - 1);
		^this.species.fill(newSize, { |i| this.blendAt(i * factor) })
	}
	
	remove { arg item;
		var index = this.indexOf(item);
		^if ( index.notNil, {
			this.removeAt(index);
		},{
			nil
		});
	}
	removing { arg item;
		var coll = this.copy;
		coll.remove(item);
		^coll
	}
	take { arg item;
		var index = this.indexOf(item);
		^if ( index.notNil, {
			this.takeAt(index);
		},{
			nil
		});
	}
	
	// accessing
	lastIndex { ^this.size - 1 }
	middleIndex { ^(this.size - 1) div: 2 }

	first { if (this.size > 0, { ^this.at(0) }, { ^nil }) }
	last { if (this.size > 0, { ^this.at(this.size - 1) }, { ^nil }) }
	middle { if (this.size > 0, { ^this.at((this.size - 1) div: 2) }, { ^nil }) }
	
	top { ^this.last }
	putFirst { arg obj; if (this.size > 0, { ^this.put(0, obj) }) }
	putLast { arg obj; if (this.size > 0, { ^this.put(this.size - 1, obj) }) }


	// compatibility with isolated objects
	
	obtain { arg index, default; ^this[index] ? default }
	
	instill { arg index, item, default;
		var res = if (index >= this.size) { 
			this.extend(index + 1, default) 
		}{ 
			this 
		};
		^res.put(index, item)
	}

	// ordering
	pairsDo { arg function;
		forBy(0, this.size-2, 2) {|i|
			function.value(this[i], this[i+1], i);
		}
	}
	doAdjacentPairs { arg function;
		(this.size - 1).do({ arg i;
			function.value(this.at(i), this.at(i+1), i);
		})
	}
	separate { arg function;
		var list = Array.new;
		var sublist = this.species.new;
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
	unlace { arg numlists, clumpSize=1, clip=false;
		var size, list, sublist, self;
		
		size = (this.size + numlists - 1) div: numlists;
		list = this.species.fill(numlists, { this.species.new(size) });
		self = if(clip) { this.keep(this.size.trunc(clumpSize * numlists).postln)Ê} { this };
		self.do({ arg item, i;
			sublist = list.at(i div: clumpSize % numlists);
			sublist.add(item);
		});
		^list
	}
	
	integrate {
		var list, sum = 0;
		list = this.class.new(this.size);
		this.do {|item|
			sum = sum + item;
			list.add( sum );
		};
		^list
	}
	differentiate {
		var list, prev = 0;
		list = this.class.new(this.size);
		this.do {|item|
			list.add( item - prev );
			prev = item;
		};
		^list
	}
		
	// pitch operations
	degreeToKey { arg scale, stepsPerOctave=12;
		var size;
		size = scale.size;
		^this.collect({ arg scaleDegree;
			scaleDegree.degreeToKey(scale, stepsPerOctave);
		});
	}
	keyToDegree { arg scale, stepsPerOctave=12;
		var key, n;
		n = this div: stepsPerOctave * scale.size;
		key = this % stepsPerOctave;
		^scale.indexInBetween(key) + n
	}
	
	nearestInScale { arg scale, stepsPerOctave=12; // collection is sorted
		var key, root;
		root = this.trunc(stepsPerOctave);
		key = this % stepsPerOctave;
		^key.nearestInList(scale) + root
	}
	nearestInList { arg list;  // collection is sorted
		^this.collect({ arg item; list.at(list.indexIn(item)) })
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
	
	nthPrime { ^this.performUnaryOp('nthPrime') }
	prevPrime { ^this.performUnaryOp('prevPrime') }
	nextPrime { ^this.performUnaryOp('nextPrime') }
	
	real { ^this.performUnaryOp('real') }
	imag { ^this.performUnaryOp('imag') }

	magnitude { ^this.performUnaryOp('magnitude') }
	magnitudeApx { ^this.performUnaryOp('magnitudeApx') }
	phase { ^this.performUnaryOp('phase') }
	angle { ^this.performUnaryOp('angle') }

	rho { ^this.performUnaryOp('rho') }
	theta { ^this.performUnaryOp('theta') }
	
	// binary math ops
	+ { arg aNumber, adverb; ^this.performBinaryOp('+', aNumber, adverb) }
	- { arg aNumber, adverb; ^this.performBinaryOp('-', aNumber, adverb) }
	* { arg aNumber, adverb; ^this.performBinaryOp('*', aNumber, adverb) }
	/ { arg aNumber, adverb; ^this.performBinaryOp('/', aNumber, adverb) }
	div { arg aNumber, adverb; ^this.performBinaryOp('div', aNumber, adverb) }
	mod { arg aNumber, adverb; ^this.performBinaryOp('mod', aNumber, adverb) }
	pow { arg aNumber, adverb; ^this.performBinaryOp('pow', aNumber, adverb) }
	min { arg aNumber, adverb; ^this.performBinaryOp('min', aNumber, adverb) } 
	max { arg aNumber=0, adverb; ^this.performBinaryOp('max', aNumber, adverb) }

	
	<  { arg aNumber, adverb; ^this.performBinaryOp('<', aNumber, adverb) }
	<= { arg aNumber, adverb; ^this.performBinaryOp('<=', aNumber, adverb) }
	>  { arg aNumber, adverb; ^this.performBinaryOp('>', aNumber, adverb) }
	>= { arg aNumber, adverb; ^this.performBinaryOp('>=', aNumber, adverb) }

	bitAnd { arg aNumber, adverb; ^this.performBinaryOp('bitAnd', aNumber, adverb) }
	bitOr { arg aNumber, adverb; ^this.performBinaryOp('bitOr', aNumber, adverb) }
	bitXor { arg aNumber, adverb; ^this.performBinaryOp('bitXor', aNumber, adverb) }	
	lcm { arg aNumber, adverb; ^this.performBinaryOp('lcm', aNumber, adverb) }
	gcd { arg aNumber, adverb; ^this.performBinaryOp('gcd', aNumber, adverb) }
	round { arg aNumber=1, adverb; ^this.performBinaryOp('round', aNumber, adverb) }
	roundUp { arg aNumber=1, adverb; ^this.performBinaryOp('roundUp', aNumber, adverb) }
	trunc { arg aNumber=1, adverb; ^this.performBinaryOp('trunc', aNumber, adverb) }
	atan2 { arg aNumber, adverb; ^this.performBinaryOp('atan2', aNumber, adverb) }	
	hypot { arg aNumber, adverb; ^this.performBinaryOp('hypot', aNumber, adverb) }
	hypotApx { arg aNumber, adverb; ^this.performBinaryOp('hypotApx', aNumber, adverb) }
	leftShift { arg aNumber, adverb; ^this.performBinaryOp('leftShift', aNumber, adverb) }
	rightShift { arg aNumber, adverb; ^this.performBinaryOp('rightShift', aNumber, adverb) }
	unsignedRightShift { arg aNumber, adverb; ^this.performBinaryOp('unsignedRightShift', aNumber, adverb) }
	ring1 { arg aNumber, adverb; ^this.performBinaryOp('ring1', aNumber, adverb) }
	ring2 { arg aNumber, adverb; ^this.performBinaryOp('ring2', aNumber, adverb) }
	ring3 { arg aNumber, adverb; ^this.performBinaryOp('ring3', aNumber, adverb) }
	ring4 { arg aNumber, adverb; ^this.performBinaryOp('ring4', aNumber, adverb) }
	difsqr { arg aNumber, adverb; ^this.performBinaryOp('difsqr', aNumber, adverb) }
	sumsqr { arg aNumber, adverb; ^this.performBinaryOp('sumsqr', aNumber, adverb) }
	sqrsum { arg aNumber, adverb; ^this.performBinaryOp('sqrsum', aNumber, adverb) }
	sqrdif { arg aNumber, adverb; ^this.performBinaryOp('sqrdif', aNumber, adverb) }
	absdif { arg aNumber, adverb; ^this.performBinaryOp('absdif', aNumber, adverb) }
	thresh { arg aNumber, adverb; ^this.performBinaryOp('thresh', aNumber, adverb) }
	amclip { arg aNumber, adverb; ^this.performBinaryOp('amclip', aNumber, adverb) }
	scaleneg { arg aNumber, adverb; ^this.performBinaryOp('scaleneg', aNumber, adverb) }
	clip2 { arg aNumber, adverb; ^this.performBinaryOp('clip2', aNumber, adverb) }
	fold2 { arg aNumber, adverb; ^this.performBinaryOp('fold2', aNumber, adverb) }
	wrap2 { arg aNumber, adverb; ^this.performBinaryOp('wrap2', aNumber, adverb) }
	excess { arg aNumber, adverb; ^this.performBinaryOp('excess', aNumber, adverb) }
	firstArg { arg aNumber, adverb; ^this.performBinaryOp('firstArg', aNumber, adverb) }
	rrand { arg aNumber, adverb; ^this.performBinaryOp('rrand', aNumber, adverb) }
	exprand { arg aNumber, adverb; ^this.performBinaryOp('exprand', aNumber, adverb) }
	
	// math op dispatch support
	performUnaryOp { arg aSelector;
		^this.collect({ arg item; item.perform(aSelector) });
	}

	performBinaryOp { arg aSelector, theOperand, adverb;
 		^theOperand.performBinaryOpOnSeqColl(aSelector, this, adverb);
	}
	performBinaryOpOnSeqColl { arg aSelector, theOperand, adverb;
		var size, newList;
		if (adverb == nil) {
			size = this.size max: theOperand.size;
			newList = this.species.new(size);
			size.do({ arg i;
				newList.add(theOperand.wrapAt(i).perform(aSelector, this.wrapAt(i)));
			});
			^newList
		};
		if (adverb.isInteger) {
			if (adverb == 0) {
				size = this.size max: theOperand.size;
				newList = this.species.new(size);
				size.do({ arg i;
					newList.add(theOperand.wrapAt(i).perform(aSelector, this.wrapAt(i)));
				});
				^newList
			}{
				if (adverb > 0) {
					^theOperand.collect {|item, i| item.perform(aSelector, this, adverb-1) }
				}{
					^this.collect {|item, i| theOperand.perform(aSelector, item, adverb+1) }
				}
			}
		};
		if (adverb == 't') {
//			size = this.size;
//			newList = this.species.new(size);
//			size.do({ arg i;
//				newList.add(theOperand.perform(aSelector, this.at(i)));
//			});
//			^newList
			size = theOperand.size;
			newList = this.species.new(size);
			size.do({ arg i;
				newList.add(theOperand.at(i).perform(aSelector, this));
			});
			^newList
		};
		if (adverb == 'x') {
//			size = this.size;
//			newList = this.species.new(size);
//			size.do({ arg i;
//				newList.add(theOperand.perform(aSelector, this.at(i)));
//			});
//			^newList
			size = theOperand.size * this.size;
			newList = this.species.new(size);
			theOperand.do({ arg a;
				this.do({ arg b;
					newList.add(a.perform(aSelector, b));
				});
			});
			^newList
		};
		if (adverb == 's') {
			size = this.size min: theOperand.size;
			newList = this.species.new(size);
			size.do({ arg i;
				newList.add(theOperand.wrapAt(i).perform(aSelector, this.wrapAt(i)));
			});
			^newList
		};
		if (adverb == 'f') {
			size = this.size max: theOperand.size;
			newList = this.species.new(size);
			size.do({ arg i;
				newList.add(theOperand.foldAt(i).perform(aSelector, this.foldAt(i)));
			});
			^newList
		};
		error("unrecognized adverb: '" ++ adverb ++ "' for operator '" ++ aSelector ++ "'\n");
		^nil
	}
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb; 
		^this.collect({ arg item; 
			aNumber.perform(aSelector, item, adverb)
		}) 
	}
	performBinaryOpOnComplex { arg aSelector, aComplex, adverb; 
		^this.collect({ arg item; 
			aComplex.perform(aSelector, item, adverb)
		}) 
	}
	clip { arg lo, hi; ^this.collect {|item| item.clip(lo,hi) }  }
	wrap { arg lo, hi; ^this.collect {|item| item.wrap(lo,hi) }  }
	fold { arg lo, hi; ^this.collect {|item| item.fold(lo,hi) }  }
	

	linlin { arg inMin, inMax, outMin, outMax, clip=\minmax; 
		^this.collect {|item| item.linlin(inMin, inMax, outMin, outMax, clip) }  
	}
	linexp { arg inMin, inMax, outMin, outMax, clip=\minmax; 
		^this.collect {|item| item.linexp(inMin, inMax, outMin, outMax, clip) }  
	}
	explin { arg inMin, inMax, outMin, outMax, clip=\minmax; 
		^this.collect {|item| item.explin(inMin, inMax, outMin, outMax, clip) }  
	}
	expexp { arg inMin, inMax, outMin, outMax, clip=\minmax; 
		^this.collect {|item| item.expexp(inMin, inMax, outMin, outMax, clip) }  
	}

	asFraction { arg denominator=100, fasterBetter=true; 
		^this.collect { |item| item.asFraction(denominator, fasterBetter) } 
	}

	asPoint { ^Point(this[0] ? 0, this[1] ? 0) }
	asRect { ^Rect(this[0] ? 0, this[1] ? 0, this[2] ? 0, this[3] ? 0) }

	// support UGen rate access
	
	rate { 
		var rate, rates;
		if(this.size == 1, { ^this.first.rate });
		^this.collect({ arg item; item.rate }).minItem; 
		// 'scalar' > 'control' > 'audio'
	}
		
	
	// support UGen range
	
		
	range { arg lo = 0.0, hi = 1.0;
		^this.multiChannelPerform(\range, lo, hi)
	}
	exprange { arg lo = 0.0, hi = 1.0;
		^this.multiChannelPerform(\exprange, lo, hi)
	}
	
	
	// UGen support
	
	lag { arg lagTime=0.1; ^this.collect { arg item; item.lag(lagTime) } }
	lag2 { arg lagTime=0.1; ^this.collect { arg item; item.lag2(lagTime) } }
	lag3 { arg lagTime=0.1; ^this.collect { arg item; item.lag3(lagTime) } }
	minNyquist { ^min(this, SampleRate.ir * 0.5) }
	
	// sorting
	sort { arg function; 
		if (function.isNil) { function = { arg a, b; a <= b }; }; 
		^this.mergeSort(function)
	}
	sortBy { arg key;
		^this.sort({| a, b | a[key] <= b[key] })
	}
	sortMap { arg function;
		^this.sort({| a, b | function.value(a) <= function.value(b) })
	}
	median { arg function;
		var sorted, index;
		sorted = this.sort(function);
		if (this.size.odd) {
			^sorted.middle 
		}{
			index = sorted.middleIndex;
			^(sorted[index] + sorted[index+1]) / 2;
		}
	}
	
	quickSort { arg function; 
		this.quickSortRange(0, this.size - 1, function) 
	}
	order { arg function; 
		var array, orderFunc;
		// returns an array of indices that would sort the collection into order.
		if (function.isNil) { function = { arg a, b; a <= b }; };
		array = [this, (0..this.lastIndex)].flop;
		orderFunc = {|a,b| function.value(a[0], b[0]) };
		^array.mergeSort(orderFunc).flop[1]
	}
	
	swap { arg i, j;
		var temp;
		temp = this[i];
		this[i] = this[j];
		this[j] = temp;
	}
	
	quickSortRange { arg i, j, function;
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
				this.quickSortRange(i, l, function);
				this.quickSortRange(k, j, function);
			});
		});
	}
	
	
	
	mergeSort { arg function;
		var tempArray;
		tempArray = this.class.newClear(this.size);
		this.mergeSortTemp(function, tempArray, 0, this.size - 1);
	}
	
	mergeSortTemp { arg function, tempArray, left, right;
		var mid, size;
		
		size = right - left;
		if (size <= 0) { ^this };
		if (size <= 8) { ^this.insertionSortRange(function, left, right) };
		
		mid = (right + left) >> 1;
		this.mergeSortTemp(function, tempArray, left, mid);
		this.mergeSortTemp(function, tempArray, mid+1, right);
		this.mergeTemp(function, tempArray, left, mid+1, right);
	}
	
	mergeTemp { arg function, tempArray, left, mid, right;
		var i, leftEnd, size, tempPos;
		leftEnd = mid - 1;
		tempPos = left;
		size = right - left + 1;
		while { (left <= leftEnd) && (mid <= right) }
		{
			if (function.value( this[left], this[mid] ))
			{
				tempArray[tempPos] = this[left];
				tempPos = tempPos + 1;
				left = left + 1;
			} 
			{
				tempArray[tempPos] = this[mid];
				tempPos = tempPos + 1;
				mid = mid + 1;
			}
		};
		while { left <= leftEnd }
		{
			tempArray[tempPos] = this[left];
			tempPos = tempPos + 1;
			left = left + 1;
		};
		while { mid <= right }
		{
			tempArray[tempPos] = this[mid];
			tempPos = tempPos + 1;
			mid = mid + 1;
		};
		size.do { 
			this[right] = tempArray[right]; 
			right = right - 1; 
		};
	}
	
	insertionSort { arg function;
		^this.insertionSortRange(function, 0, this.size - 1)
	}
	insertionSortRange { arg function, left, right;
		var i, j, test;
		i = left + 1;
		while { i <= right }
		{
			test = this[i];
			j = i;
			while { (j > left) && { function.value(this[j-1], test).not } }
			{
				this[j] = this[j-1];
				j = j - 1;
			};
			this[j] = test;
			i = i + 1;
		}
	}

	// mirror image of String-split
	join { arg joiner;
		^String.streamContents({ arg stream;
			var stop;
			if(joiner.isNil,{
				this.do({ arg item; stream << item });
			},{
				stop = this.size - 1;
				this.do({ arg item,i;
					stream << item;
					if(i < stop,{ stream << joiner; });
				});
			})
		})
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
	
	wrapAt { arg index;
		index = index % this.size;
		^this.at(index)
	}
	wrapPut { arg index, value;
		index = index % this.size;
		^this.put(index, value)
	}
	reduce { arg operator;
		var once = true, result;
		this.doAdjacentPairs {|a, b|
			if (once) {
				once = false;
				result = a.perform(operator, b);
			}{
				result = result.perform(operator, b)
			};
		};
		^result
	}

	// TempoClock play quantization
	nextTimeOnGrid { arg clock;
		^clock.nextTimeOnGrid(*this);
	}
}
