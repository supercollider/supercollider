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

	// fill with interpolation of values between start and end
	*interpolation { arg size, start=0.0, end=1.0;
		var obj = this.new(size), step;
		if(size == 1) { ^obj.add(start) };
		step = (end - start) / (size - 1);
		size.do {|i|
			obj.add(start + (i * step));
		};
		^obj
	}


	++ { arg aSequenceableCollection;
		var newlist = this.species.new(this.size + aSequenceableCollection.size);
		newlist = newlist.addAll(this).addAll(aSequenceableCollection);
		^newlist
	}
	+++ { arg aSequenceableCollection, adverb;
		aSequenceableCollection = aSequenceableCollection.asSequenceableCollection;
		if(adverb.notNil) {
			if(adverb == 0) { ^this ++ aSequenceableCollection };
			if(adverb == 1) { ^this +++ aSequenceableCollection };
			if(adverb < 0) { ^aSequenceableCollection.perform('+++', this, adverb.neg) };
			^this.deepCollect(adverb - 1, { |sublist|
				sublist.asSequenceableCollection.collect {|item, i|
					item.asSequenceableCollection ++ aSequenceableCollection.wrapAt(i)
				}
			})
		};
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
		if (this === aCollection) { ^true };
		if (aCollection.class != this.class) { ^false };
		if (this.size != aCollection.size) { ^false };
		this.do { | item, i |
			if (item != aCollection[i]) { ^false };
		};
		^true
	}

	hash {
		var hash = this.class.hash;
		this.do { | item |
			hash = hash << 1 bitXor: item.hash // encode item order by left shifting
		};
		^hash
	}

	copyRange { arg start, end;
		var newColl;
		var i = start;
		newColl = this.species.new(end - start);
		while ({ i <= end },{
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

	indexOfEqual { arg item, offset=0;
		(this.size - offset).do ({ arg i;
			i = i + offset;
			if ( item == this[i], { ^i })
		});
		^nil
	}

	indicesOfEqual { |item|
		var indices;
		this.do { arg val, i;
			if (item == val) { indices = indices.add(i) }
		};
		^indices
	}


	find { |sublist, offset=0|
		var subSize_1 = sublist.size - 1, first = sublist.first, index;
		(this.size - offset).do { |i|
			index = i + offset;
			if (this[index] == first) {
				if (this.copyRange(index, index + subSize_1) == sublist) { ^index }
			};
		};
		^nil
	}

	findAll { arg arr, offset=0;
		var indices, i=0;
		while {
			i = this.find(arr, offset);
			i.notNil
		}{
			indices = indices.add(i);
			offset = i + 1;
		}
		^indices
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
		var a, b, div, i;
		if(this.isEmpty) { ^nil };
		i = this.indexOfGreaterThan(val);
		if(i.isNil) { ^this.size - 1 };
		if(i == 0) { ^i };
		a = this[i-1]; b = this[i];
		div = b - a;
		if(div == 0) { ^i };
		^((val - a) / div) + i - 1
	}

	selectIndices { | function |
		^this.selectIndicesAs(function, this.species);
	}
	selectIndicesAs { | function, class |
		var res = class.new(this.size);
		this.do {|elem, i| if (function.value(elem, i)) { res.add(i) } }
		^res;
	}

	rejectIndices { | function |
		^this.rejectIndicesAs(function, this.species);
	}
	rejectIndicesAs { | function, class |
		var res = class.new(this.size);
		this.do {|elem, i| if (function.value(elem, i).not) { res.add(i) } }
		^res;
	}

	isSeries { arg step;
		if(this.size <= 1) { ^true };
		this.doAdjacentPairs { |a, b|
			var diff = b - a;
			if(step.isNil) { step = diff } {
				if(step != diff) { ^false }
			}
		};
		^true
	}

	resamp0 { arg newSize;
		var factor = this.size - 1 / (newSize - 1).max(1);
		^this.species.fill(newSize, { |i| this.at((i * factor).round(1.0).asInteger) })
	}

	resamp1 { arg newSize;
		var factor = this.size - 1 / (newSize - 1).max(1);
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
	lastIndex { ^if(this.size > 0) { this.size - 1 } { nil } }
	middleIndex { ^if(this.size > 0) { (this.size - 1) div: 2 } { nil } }

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
			this.copy
		};
		^res.put(index, item)
	}

	// ordering
	pairsDo { arg function;
		forBy(0, this.size-2, 2) {|i|
			function.value(this[i], this[i+1], i);
		}
	}
	keysValuesDo { arg function;
		^this.pairsDo(function)
	}

	doAdjacentPairs { arg function;
		(this.size - 1).do({ arg i;
			function.value(this.at(i), this.at(i+1), i);
		})
	}
	separate { arg function = true;
		var list = Array.new;
		var sublist = this.species.new;
		this.doAdjacentPairs({ arg a, b, i;
			sublist = sublist.add(a);
			if ( function.value(a, b, i), {
				list = list.add(sublist);
				sublist = this.species.new;
			});
		});
		if(this.notEmpty) { sublist = sublist.add(this.last) };
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
		var list = Array.new((this.size / groupSize).roundUp.asInteger);
		var sublist = this.species.new(groupSize);
		this.do({ arg item;
			sublist.add(item);
			if (sublist.size >= groupSize, {
				list.add(sublist);
				sublist = this.species.new(groupSize);
			});
		});
		if (sublist.size > 0, { list = list.add(sublist); });
		^list
	}
	clumps { arg groupSizeList;
		var i = 0;
		var list = Array.new(groupSizeList.size); // still better estimate than default
		var subSize = groupSizeList.at(0);
		var sublist = this.species.new(subSize);
		this.do({ arg item;
			sublist = sublist.add(item);
			if (sublist.size >= subSize, {
				i = i + 1;
				list = list.add(sublist);
				subSize = groupSizeList.wrapAt(i);
				sublist = this.species.new(subSize);
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

		list = this.species.new(this.size);
		this.do({ arg item;
			if (item.respondsTo('flatten'), {
				list = list.addAll(item.flatten(numLevels));
			},{
				list = list.add(item);
			});
		});
		^list
	}

	flatBelow { |level = 1|

		if (level <=0) { ^this.flat };
		level = level - 1;
		^this.collect { |item|
			if (item.respondsTo(\flatBelow)) {
				item.flatBelow(level)
			} {
				item
			}
		}
	}

	// bidirectional flattening
	flatten2 { arg numLevels=1;
		if (numLevels == 0) { ^this };
		if (numLevels > 0) { ^this.flatten(numLevels) };
		^this.flatBelow(this.maxDepth - 1 + numLevels);
	}

	flat {
		^this.prFlat(this.species.new(this.flatSize))
	}

	prFlat { |list|
		this.do({ arg item, i;
			if (item.respondsTo('prFlat'), {
				list = item.prFlat(list);
			},{
				list = list.add(item);
			});
		});
		^list
	}

	flatIf { |func|
		var list = this.species.new(this.size); // as we don't know the size, just guess
		this.do({ arg item, i;
			if (item.respondsTo('flatIf') and: { func.value(item, i) }, {
				list = list.addAll(item.flatIf(func));
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
			sz = if(sublist.isSequenceableCollection, { sublist.size }, { 1 });
			if (sz > maxsize, { maxsize = sz });
		});

		list = this.species.fill(maxsize, { this.species.new(size) });
		this.do({ arg isublist, i;
			if(isublist.isSequenceableCollection, {
				list.do({ arg jsublist, j;
					jsublist.add( isublist.wrapAt(j) );
				});
			},{
				list.do({ arg jsublist, j;
					jsublist.add( isublist );
				});
			});
		});
		^list
	}

	flopWith { |func|
		var maxsize = this.maxValue { |sublist|
			if(sublist.isSequenceableCollection) { sublist.size } { 1 }
		};

		^this.species.fill(maxsize, { |i|
			func.value( *this.collect { |sublist|
				if(sublist.isSequenceableCollection) {
					sublist.wrapAt(i)
				} {
					sublist
				}
			})
		})
	}

	flopTogether { arg ... moreArrays;
		var standIn, maxSize = 0, array;
		array = [this] ++ moreArrays;
		array.do { |sublist|
			sublist.do { |each|
				maxSize = max(maxSize, each.size)
			}
		};
		standIn = 0.dup(maxSize);
		array = array.collect { |sublist| sublist.add(standIn) };
		^array.collect { |sublist|
			sublist.flop.collect { |each| each.drop(-1) } // remove stand-in
		};
	}

	flopDeep { arg rank;
		var size, maxsize;
		if(rank.isNil) { rank = this.maxDepth - 1 };
		if(rank <= 1) { ^this.flop };

		size = this.size;
		maxsize = this.maxSizeAtDepth(rank);
		^this.species.fill(maxsize, { |i|
			this.wrapAtDepth(rank, i)
		})
	}

	wrapAtDepth { arg rank, index;
		if(rank == 0) { ^this.wrapAt(index) };
		^this.collect { |item, i|
			if(item.isSequenceableCollection) {
				item.wrapAtDepth(rank - 1, index)
			} {
				item
			}
		}
	}

	unlace { arg numlists, clumpSize=1, clip=false;
		var size, list, sublist, self;

		size = (this.size + numlists - 1) div: numlists;
		list = this.species.fill(numlists, { this.species.new(size) });
		self = if(clip) { this.keep(this.size.trunc(clumpSize * numlists).postln)} { this };
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
	// complement to Integer:asDigits
	convertDigits { arg base=10;
		var lastIndex = this.lastIndex;
		^this.sum { |x, i|
			if(x >= base) { Error("digit too large for base").throw };
			base ** (lastIndex - i) * x
		}.asInteger
	}

	hammingDistance { |that|
		// if this is shorter than that, size difference should be included
		// (if this is longer, the do loop will take care of it)
		var	count = (that.size - this.size).max(0);
		this.do({ |elem, i|
			if(elem != that[i]) { count = count + 1 };
		});
		^count
	}

	// pitch operations
	degreeToKey { arg scale, stepsPerOctave=12;
		^this.collect({ arg scaleDegree;
			scaleDegree.degreeToKey(scale, stepsPerOctave);
		});
	}
	keyToDegree { arg scale, stepsPerOctave=12;
		^this.collect { arg val; val.keyToDegree(scale, stepsPerOctave) }
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

	performDegreeToKey { arg scaleDegree, stepsPerOctave = 12, accidental = 0;
		var baseKey = (stepsPerOctave * (scaleDegree div: this.size)) + this.wrapAt(scaleDegree);
		^if(accidental == 0) { baseKey } { baseKey + (accidental * (stepsPerOctave / 12.0)) }
	}

	performKeyToDegree { | degree, stepsPerOctave = 12 |
		var n = degree div: stepsPerOctave * this.size;
		var key = degree % stepsPerOctave;
		^this.indexInBetween(key) + n
	}

	performNearestInList { | degree |
		^this.at(this.indexIn(degree))
	}

	performNearestInScale { arg degree, stepsPerOctave=12; // collection is sorted
		var root = degree.trunc(stepsPerOctave);
		var key = degree % stepsPerOctave;
		^key.nearestInList(this) + root
	}

	// supports a variation of Mikael Laurson's rhythm list RTM-notation.
	convertRhythm {
		var list, tie;
		list = List.new;
		tie = this.convertOneRhythm(list);
		if (tie > 0.0, { list.add(tie) });  // check for tie at end of rhythm
		^list
	}
	sumRhythmDivisions {
		var sum = 0;
		this.do {|beats|
			sum = sum + abs(if (beats.isSequenceableCollection) {
				beats[0];
			}{
				beats
			});
		};
		^sum
	}
	convertOneRhythm { arg list, tie = 0.0, stretch = 1.0;
		var beats, divisions, repeats;
		#beats, divisions, repeats = this;
		repeats = repeats ? 1;
		stretch = stretch * beats / divisions.sumRhythmDivisions;
		repeats.do({
			divisions.do { |val|
				if (val.isSequenceableCollection) {
					tie = val.convertOneRhythm(list, tie, stretch)
				}{
					val = val * stretch;
					if (val > 0.0) {
						list.add(val + tie);
						tie = 0.0;
					}{
						tie = tie - val
					};
				};
			};
		});
		^tie
	}

	isSequenceableCollection { ^true }
	containsSeqColl { ^this.any(_.isSequenceableCollection) }
	isAssociationArray { ^this.at(0).isKindOf(Association) }

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
	indexOfPrime { ^this.performUnaryOp('indexOfPrime') }

	real { ^this.performUnaryOp('real') }
	imag { ^this.performUnaryOp('imag') }

	magnitude { ^this.performUnaryOp('magnitude') }
	magnitudeApx { ^this.performUnaryOp('magnitudeApx') }
	phase { ^this.performUnaryOp('phase') }
	angle { ^this.performUnaryOp('angle') }

	rho { ^this.performUnaryOp('rho') }
	theta { ^this.performUnaryOp('theta') }

	degrad { ^this.performUnaryOp('degrad') }
	raddeg { ^this.performUnaryOp('raddeg') }

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
	bitHammingDistance { arg aNumber, adverb;
		^this.performBinaryOp('hammingDistance', aNumber, adverb)
	}

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
	clip2 { arg aNumber=1, adverb; ^this.performBinaryOp('clip2', aNumber, adverb) }
	fold2 { arg aNumber=1, adverb; ^this.performBinaryOp('fold2', aNumber, adverb) }
	wrap2 { arg aNumber=1, adverb; ^this.performBinaryOp('wrap2', aNumber, adverb) }
	excess { arg aNumber=1, adverb; ^this.performBinaryOp('excess', aNumber, adverb) }
	firstArg { arg aNumber, adverb; ^this.performBinaryOp('firstArg', aNumber, adverb) }
	rrand { arg aNumber, adverb; ^this.performBinaryOp('rrand', aNumber, adverb) }
	exprand { arg aNumber, adverb; ^this.performBinaryOp('exprand', aNumber, adverb) }


	/*  Boost Special Functions  */

	//  Number Series:
	bernouliB2n { ^this.performUnaryOp('bernouliB2n') }
	tangentT2n { ^this.performUnaryOp('tangentT2n') }

	//  Gamma:
	tgamma { ^this.performUnaryOp('tgamma') }
	tgamma1pm1 { ^this.performUnaryOp('tgamma1pm1') }
	lgamma { ^this.performUnaryOp('lgamma') }
	digamma { ^this.performUnaryOp('digamma') }
	trigamma { ^this.performUnaryOp('trigamma') }
	polygamma { |n, z| ^this.multiChannelPerform('polygamma', n, z) }
	tgammaRatio { |a, b| ^this.multiChannelPerform('tgammaRatio', a, b) }
	tgammaDeltaRatio { |a, delta| ^this.multiChannelPerform('tgammaDeltaRatio', a, delta) }
	gammaP { |a, z| ^this.multiChannelPerform('gammaP', a, z) }
	gammaQ { |a, z| ^this.multiChannelPerform('gammaQ', a, z) }
	tgammaLower { |a, z| ^this.multiChannelPerform('tgammaLower', a, z) }
	tgammaUpper { |a, z| ^this.multiChannelPerform('tgammaUpper', a, z) }
	//  Incomplete Gamma Function Inverses
	gammaPInv { |a, p| ^this.multiChannelPerform('gammaPInv', a, p) }
	gammaQInv { |a, q| ^this.multiChannelPerform('gammaQInv', a, q) }
	gammaPInvA { |x, p| ^this.multiChannelPerform('gammaPInvA', x, p) }
	gammaQInvA { |x, q| ^this.multiChannelPerform('gammaQInvA', x, q) }
	//  Derivatives of the Incomplete Gamma Function
	gammaPDerivative { |a, x| ^this.multiChannelPerform('gammaPDerivative', a, x) }
	gammaQDerivative { |a, x| ^this.gammaPDerivative(a, x).neg }

	//	Factorials and Binomial Coefficients:
	factorial { ^this.performUnaryOp('factorial') }
	doubleFactorial { ^this.performUnaryOp('doubleFactorial') }
	risingFactorial { |x, i| ^this.multiChannelPerform('risingFactorial', x, i) }
	fallingFactorial { |x, i| ^this.multiChannelPerform('fallingFactorial', x, i) }
	binomialCoefficient { |n, k| ^this.multiChannelPerform('binomialCoefficient', n, k) }

	//  Beta functions:
	beta { |a, b| ^this.multiChannelPerform('beta', a, b) }
	//  Incomplete beta functions
	ibeta { |... args| ^this.multiChannelPerform('ibeta', *args) }
	ibetaC { |... args| ^this.multiChannelPerform('ibetaC', *args) }
	betaFull { |... args| ^this.multiChannelPerform('betaFull', *args) }
	betaFullC { |... args| ^this.multiChannelPerform('betaFullC', *args) }
	//  Incomplete beta function inverses
	ibetaInv { |... args| ^this.multiChannelPerform('ibetaInv', *args) }
	ibetaCInv { |... args| ^this.multiChannelPerform('ibetaCInv', *args) }
	ibetaInvA { |... args| ^this.multiChannelPerform('ibetaInvA', *args) }
	ibetaCInvA { |... args| ^this.multiChannelPerform('ibetaCInvA', *args) }
	ibetaInvB { |... args| ^this.multiChannelPerform('ibetaInvB', *args) }
	ibetaCInvB { |... args| ^this.multiChannelPerform('ibetaCInvB', *args) }
	//  Incomplete beta function derivative
	ibetaDerivative { |... args| ^this.multiChannelPerform('ibetaDerivative', *args) }

	//  Error functions:
	erf { ^this.performUnaryOp('erf') }
	erfC { ^this.performUnaryOp('erfC') }
	erfInv { ^this.performUnaryOp('erfInv') }
	erfCInv { ^this.performUnaryOp('erfCInv') }

	//  Polynomials:
	// Legendre (and Associated), Laguerre (and Associated),
	// Hermite, Chebyshev Polynomials (first & second kind, derivative, zero (root) finder)
	// See boost documentation regarding the Condon-Shortley phase term of (-1)^m
	// "http://www.boost.org/doc/libs/1_65_1/libs/math/doc/html/math_toolkit/sf_poly/legendre.html"]
	legendreP { |n, x| ^this.multiChannelPerform('legendreP', n, x) }
	legendrePPrime { |n, x| ^this.multiChannelPerform('legendrePPrime', n, x) }
	legendrePZeros { ^this.performUnaryOp('legendrePZeros') }
	legendrePAssoc { |... args| ^this.multiChannelPerform('legendrePAssoc', *args) }
	legendreQ { |n, x| ^this.multiChannelPerform('legendreQ', n, x) }
	laguerre { |n, x| ^this.multiChannelPerform('laguerre', n, x) }
	laguerreAssoc { |... args| ^this.multiChannelPerform('laguerreAssoc', *args) }
	hermite { |n, x| ^this.multiChannelPerform('hermite', n, x) }
	chebyshevT { |n, x| ^this.multiChannelPerform('chebyshevT', n, x) }
	chebyshevU { |n, x| ^this.multiChannelPerform('chebyshevU', n, x) }
	chebyshevTPrime { |n, x| ^this.multiChannelPerform('chebyshevTPrime', n, x) }
	//  "https://en.wikipedia.org/wiki/Chebyshev_polynomials#Roots_and_extrema"
	//  "http://mathworld.wolfram.com/ChebyshevPolynomialoftheFirstKind.html"
	chebyshevTZeros {
		var n = this.asInteger;
		^(1..n).collect({ arg k;
			cos(pi* ((2*k) - 1) / (2*n))
		});
	}

	//  Spherical Harmonics:
	sphericalHarmonic { |... args| ^this.multiChannelPerform('sphericalHarmonic', *args) }
	sphericalHarmonicR { |... args| ^this.multiChannelPerform('sphericalHarmonicR', *args) }
	sphericalHarmonicI { |... args| ^this.multiChannelPerform('sphericalHarmonicI', *args) }

	//	Bessel Functions:
	//  First and second kind, zero finders, modified first and second kinds,
	//  spherical first and second kinds, derivatives
	cylBesselJ { |v, x| ^this.multiChannelPerform('cylBesselJ', v, x) }
	cylNeumann { |v, x| ^this.multiChannelPerform('cylNeumann', v, x) }
	cylBesselJZero { |v, index| ^this.multiChannelPerform('cylBesselJZero', v, index) }
	cylNeumannZero { |v, index| ^this.multiChannelPerform('cylNeumannZero', v, index) }
	cylBesselI { |v, x| ^this.multiChannelPerform('cylBesselI', v, x) }
	cylBesselK { |v, x| ^this.multiChannelPerform('cylBesselK', v, x) }
	sphBessel { |v, x| ^this.multiChannelPerform('sphBessel', v, x) }
	sphNeumann { |v, x| ^this.multiChannelPerform('sphNeumann', v, x) }
	cylBesselJPrime { |v, x| ^this.multiChannelPerform('cylBesselJPrime', v, x) }
	cylNeumannPrime { |v, x| ^this.multiChannelPerform('cylNeumannPrime', v, x) }
	cylBesselIPrime { |v, x| ^this.multiChannelPerform('cylBesselIPrime', v, x) }
	cylBesselKPrime { |v, x| ^this.multiChannelPerform('cylBesselKPrime', v, x) }
	sphBesselPrime { |v, x| ^this.multiChannelPerform('sphBesselPrime', v, x) }
	sphNeumannPrime { |v, x| ^this.multiChannelPerform('sphNeumannPrime', v, x) }

	//  Hankel Functions:
	cylHankel1 { |v, x| ^this.multiChannelPerform('cylHankel1', v, x) }
	cylHankel2 { |v, x| ^this.multiChannelPerform('cylHankel2', v, x) }
	sphHankel1 { |v, x| ^this.multiChannelPerform('sphHankel1', v, x) }
	sphHankel2 { |v, x| ^this.multiChannelPerform('sphHankel2', v, x) }

	//  Airy Functions:
	airyAi { ^this.performUnaryOp('airyAi') }
	airyBi { ^this.performUnaryOp('airyBi') }
	airyAiPrime { ^this.performUnaryOp('airyAiPrime') }
	airyBiPrime { ^this.performUnaryOp('airyBiPrime') }
	airyAiZero { ^this.performUnaryOp('airyAiZero') }
	airyBiZero { ^this.performUnaryOp('airyBiZero') }

	//  Elliptic Integrals:
	//  Carlson Form
	ellintRf { |... args| ^this.multiChannelPerform('ellintRf', *args) }
	ellintRd { |... args| ^this.multiChannelPerform('ellintRd', *args) }
	ellintRj { |... args| ^this.multiChannelPerform('ellintRj', *args) }
	ellintRc { |x, y| ^this.multiChannelPerform('ellintRc', x, y) }
	ellintRg { |... args| ^this.multiChannelPerform('ellintRg', *args) }
	//  Legendre Form - First, Second, Third Kind, D
	ellint1 { |k, phi| ^this.multiChannelPerform('ellint1', k, phi) }
	ellint1C { ^this.performUnaryOp('ellint1C') }
	ellint2 { |k, phi| ^this.multiChannelPerform('ellint2', k, phi) }
	ellint2C { ^this.performUnaryOp('ellint2C') }
	ellint3 { |... args| ^this.multiChannelPerform('ellint3', *args) }
	ellint3C { |k, n| ^this.multiChannelPerform('ellint3C', k, n) }
	ellintD { |k, phi| ^this.multiChannelPerform('ellintD', k, phi) }
	ellintDC { ^this.performUnaryOp('ellintDC') }
	//  Jacobi Zeta, Heuman Lambda Functions
	jacobiZeta { |k, phi| ^this.multiChannelPerform('jacobiZeta', k, phi) }
	heumanLambda { |k, phi| ^this.multiChannelPerform('heumanLambda', k, phi) }

	//  Jacobi Elliptic Functions:
	jacobiCd { |k, u| ^this.multiChannelPerform('jacobiCd', k, u) }
	jacobiCn { |k, u| ^this.multiChannelPerform('jacobiCn', k, u) }
	jacobiCs { |k, u| ^this.multiChannelPerform('jacobiCs', k, u) }
	jacobiDc { |k, u| ^this.multiChannelPerform('jacobiDc', k, u) }
	jacobiDn { |k, u| ^this.multiChannelPerform('jacobiDn', k, u) }
	jacobiDs { |k, u| ^this.multiChannelPerform('jacobiDs', k, u) }
	jacobiNc { |k, u| ^this.multiChannelPerform('jacobiNc', k, u) }
	jacobiNd { |k, u| ^this.multiChannelPerform('jacobiNd', k, u) }
	jacobiNs { |k, u| ^this.multiChannelPerform('jacobiNs', k, u) }
	jacobiSc { |k, u| ^this.multiChannelPerform('jacobiSc', k, u) }
	jacobiSd { |k, u| ^this.multiChannelPerform('jacobiSd', k, u) }
	jacobiSn { |k, u| ^this.multiChannelPerform('jacobiSn', k, u) }

	//  Riemann Zeta Function:
	zeta { ^this.performUnaryOp('zeta') }

	//  Exponential Integrals:
	expintEn { |n, z| ^this.multiChannelPerform('expintEn', n, z) }
	expintEi { ^this.performUnaryOp('expintEi') }

	//  Basic Functions:
	sinPi { ^this.performUnaryOp('sinPi') }
	cosPi { ^this.performUnaryOp('cosPi') }
	log1p { ^this.performUnaryOp('log1p') }
	expm1 { ^this.performUnaryOp('expm1') }
	cbrt { ^this.performUnaryOp('cbrt') }
	sqrt1pm1 { ^this.performUnaryOp('sqrt1pm1') }
	powm1 { |x, y| ^this.multiChannelPerform('powm1', x, y) }
	// hypot not needed... slightly slower than current implementation of hypot

	//  Sinus Cardinal ("sinc") and Hyperbolic Sinus Cardinal Functions:
	sincPi { ^this.performUnaryOp('sincPi') }
	sinhcPi { ^this.performUnaryOp('sinhcPi') }

	//  Inverse Hyperbolic Functions:
	asinh { ^this.performUnaryOp('asinh') }
	acosh { ^this.performUnaryOp('acosh') }
	atanh { ^this.performUnaryOp('atanh') }

	//	Owen's T function:
	owensT { |h, a| ^this.multiChannelPerform('owensT', h, a) }

	/*  end Boost Special Functions  */


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
		Error("unrecognized adverb: '" ++ adverb ++ "' for operator '" ++ aSelector ++ "'\n").throw;
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

	asFraction { arg denominator=100, fasterBetter=true;
		^this.collect { |item| item.asFraction(denominator, fasterBetter) }
	}

	asPoint { ^Point(this[0] ? 0, this[1] ? 0) }
	asRect { ^Rect(this[0] ? 0, this[1] ? 0, this[2] ? 0, this[3] ? 0) }
	ascii { ^this.collect { arg item; item.ascii } }


	// support UGen rate access

	rate {
		var rate, rates;
		if(this.size == 1) { ^this.first.rate };
		^this.collect({ arg item; item.rate ? 'scalar' }).minItem
		// 'scalar' > 'control' > 'audio'
	}

	// if we don't catch the special case of an empty array,
	// Object:multiChannelPerform goes into infinite recursion
	multiChannelPerform { arg selector ... args;
		if(this.size > 0) {
			^super.multiChannelPerform(selector, *args);
		} {
			^this.class.new
		}
	}

	// this method is for UGen inputs that require Refs to block direct multichannel expansion.
	// here, we assume this is already an array of Refs, which we simply return.
	multichannelExpandRef { arg rank;
		^this
	}

	// support some UGen convenience methods.
	// NOTE: don't forget to add a wrapper here when adding a method to UGen or AbstractFunction
	clip { arg ... args; ^this.multiChannelPerform('clip', *args) }
	wrap { arg ... args; ^this.multiChannelPerform('wrap', *args) }
	fold { arg ... args; ^this.multiChannelPerform('fold', *args) }
	linlin { arg ... args; ^this.multiChannelPerform('linlin', *args) }
	linexp { arg ... args; ^this.multiChannelPerform('linexp', *args) }
	explin { arg ... args; ^this.multiChannelPerform('explin', *args) }
	expexp { arg ... args; ^this.multiChannelPerform('expexp', *args) }
	lincurve { arg ... args; ^this.multiChannelPerform('lincurve', *args) }
	curvelin { arg ... args; ^this.multiChannelPerform('curvelin', *args) }
	bilin { arg ... args; ^this.multiChannelPerform('bilin', *args) }
	biexp { arg ... args; ^this.multiChannelPerform('biexp', *args) }
	moddif { arg ... args; ^this.multiChannelPerform('moddif', *args) }
	range { arg ... args; ^this.multiChannelPerform('range', *args) }
	exprange { arg ... args; ^this.multiChannelPerform('exprange', *args) }
	curverange { arg ... args; ^this.multiChannelPerform('curverange', *args) }
	unipolar { arg ... args; ^this.multiChannelPerform('unipolar', *args) }
	bipolar { arg ... args; ^this.multiChannelPerform('bipolar', *args) }
	lag { arg ... args; ^this.multiChannelPerform('lag', *args) }
	lag2 { arg ... args; ^this.multiChannelPerform('lag2', *args) }
	lag3 { arg ... args; ^this.multiChannelPerform('lag3', *args) }
	lagud { arg ... args; ^this.multiChannelPerform('lagud', *args) }
	lag2ud { arg ... args; ^this.multiChannelPerform('lag2ud', *args) }
	lag3ud { arg ... args; ^this.multiChannelPerform('lag3ud', *args) }
	varlag { arg ... args; ^this.multiChannelPerform('varlag', *args) }
	slew { arg ... args; ^this.multiChannelPerform('slew', *args) }
	blend { arg ... args; ^this.multiChannelPerform('blend', *args) }
	checkBadValues { arg ... args; ^this.multiChannelPerform('checkBadValues', *args) }
	prune { arg ... args; ^this.multiChannelPerform('prune', *args) }

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
	sortedMedian {
		var index;
		if (this.size.odd) {
			^this.middle
		}{
			index = this.middleIndex;
			^(this[index] + this[index+1]) / 2;
		}
	}
	median { arg function;
		//^this.sort(function).sortedMedian
		// Note the copy, to prevent changing the input.
		^this.copy.hoareMedian(function)
	}

	quickSort { arg function;
		this.quickSortRange(0, this.size - 1, function)
	}
	order { arg function;
		var array, orderFunc;
		// returns an array of indices that would sort the collection into order.
		if(this.isEmpty) { ^[] };
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


	// Finds the median efficiently, by rearranging the array IN-PLACE.
	hoareMedian { |function|
		if(this.isEmpty) { ^nil };
		^if(this.size.even, {
			[this.hoareFind(this.size/ 2 - 1, function),
				this.hoareFind(this.size/ 2,     function)].mean;
		}, {
			this.hoareFind(this.size - 1 / 2, function);
		});
	}

	// Finds the kth element in the array, according to a given sorting function.
	// This is typically fast (order is O(n) rather than O(n log n)) because it
	// doesn't attempt to completely sort the array. Method is due to C. A. F. Hoare.
	// Note: this rearranges array elements IN PLACE.
	hoareFind { |k, function, left, right|
		var i,j,p,r,l;

		if (function.isNil) { function = { | a, b | a < b } };

		i = left  ?  0;
		j = right ?? {this.size-1};

		while{ i < j }{
			p = this[k];
			# l, r = this.hoarePartition(i,j,p, function);
			if(r < k, {
				// kth smallest is in right split
				i = l;
			});
			if(k < l, {
				// kth smallest is in left split
				j = r;
			});
		};
		// The desired element is in desired position
		^this[k];
	}

	// In-place partitioning method used by hoareFind.
	// Note: for efficiency this doesn't check that function is defined, so you
	// must supply a function! See hoareFind for example
	hoarePartition { |l0, r0, p, function|
		var l, r, tmp;

		l = l0;
		r = r0;

		while({ l <= r }, {
			// left_scan
			while { (l < this.size) and: { function.value(this[l], p) } }{
				l = l + 1;
			};
			// right_scan
			while { (r >= 0) and: { function.value(p, this[r]) } }{
				r = r - 1;
			};
			// check and exchange
			if(l <= r){
				tmp = this[l];
				this[l] = this[r];
				this[r] = tmp;
				// then
				l = l + 1;
				r = r - 1;
			};
		});

		^[l,r];
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
	reduce { arg operator, adverb;
		var once = true, result;
		if(this.size==1){ ^this[0] };
		this.doAdjacentPairs {|a, b|
			if (once) {
				once = false;
				result = operator.applyTo(a, b, adverb);
			}{
				result =  operator.applyTo(result, b, adverb);
			};
		};
		^result
	}
	join { arg joiner;
		^String.streamContents { arg stream;
			var stop;
			if(joiner.isNil) {
				this.do { arg item; stream << item };
			} {
				stop = this.size - 1;
				this.do { arg item,i;
					stream << item;
					if(i < stop) { stream << joiner };
				};
			}
		}
	}


	// TempoClock play quantization
	nextTimeOnGrid { arg clock;
		^clock.nextTimeOnGrid(*this);
	}

	// we break up the array so that missing elements are set to nil in the Quant
	asQuant { ^Quant(*this) }

	//	asUGenInput { ^this.asArray.asUGenInput }

	// this method could be refactored by dispatching, but we're trying to keep the overhead low.

	schedBundleArrayOnClock { |clock, bundleArray, lag = 0, server, latency|

		// "this" is an array of delta times for the clock (usually in beats)
		// "lag" is a value or an array of tempo independent absolute lag times (in seconds)

		var sendBundle;

		latency = latency ? server.latency;
		sendBundle = { |i| server.sendBundle(latency, bundleArray.wrapAt(i)) };

		if (lag != 0) {
			lag = lag.asArray;

			this.do { |delta, i|
				if(delta != 0) {
					// schedule on both clocks
					clock.sched(delta, {
						SystemClock.sched(lag.wrapAt(i), { sendBundle.value(i) })
					})
				} {
					// schedule only on the system clock
					SystemClock.sched(lag.wrapAt(i), { sendBundle.value(i) })
				}
			}
		} {
			this.do { |delta, i|
				if(delta != 0) {
					// schedule only on the clock passed in
					clock.sched(delta, { sendBundle.value(i) })
				} {
					// send directly
					sendBundle.value(i)
				}
			}
		}
	}

	unixCmd { arg action, postOutput = true;
		var pid;
		if(this.notEmpty) {
			pid = this.prUnixCmd(postOutput);
			if(action.notNil) {
				String.unixCmdActions.put(pid, action);
			};
			^pid
		} {
			Error("Collection should have at least the filepath of the program to run.").throw
		}
	}

	unixCmdGetStdOut { arg maxLineLength=1024;
		var pipe, lines, line, pid;

		if(this.notEmpty) {
			pipe = Pipe.argv(this, "r");
			lines = "";
			line = pipe.getLine(maxLineLength);
			while({line.notNil}, {lines = lines ++ line ++ "\n"; line = pipe.getLine; });
			pipe.close;
			^lines
		} {
			Error("Collection should have at least the filepath of the program to run.").throw
		}
	}

	prUnixCmd { arg postOutput = true;
		_ArrayPOpen
		^this.primitiveFailed
	}

	sanitize { arg ... args;
		^this.multiChannelPerform(\sanitize, *args);
	}
}
