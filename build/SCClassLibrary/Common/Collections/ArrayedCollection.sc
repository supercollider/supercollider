
ArrayedCollection : SequenceableCollection {
	*newClear { arg indexedSize = 0; 
		_BasicNewClear 
		^this.primitiveFailed
		// creates a new instance with indexedSize indexable slots.
		// the slots are filled with nil, zero or something else
		// appropriate to the type of indexable slots in the
		// object.
	}

	// ArrayedCollections are vectors which have a 
	// fixed maximum capacity. 

	size { _BasicSize }
	maxSize { _BasicMaxSize }
		
	swap { arg i, j; var temp;
		_BasicSwap;
		^this.primitiveFailed;
	}

	at { arg index; 
		_BasicAt;
		^this.primitiveFailed;
	}
	clipAt { arg index; 
		_BasicClipAt;
		^this.primitiveFailed;
	}
	wrapAt { arg index; 
		_BasicWrapAt;
		^this.primitiveFailed;
	}
	foldAt { arg index; 
		_BasicFoldAt;
		^this.primitiveFailed;
	}
	put { arg index, item; 
		_BasicPut;
		^this.primitiveFailed;
	}
	clipPut { arg index, item; 
		_BasicClipPut;
		^this.primitiveFailed;
	}
	wrapPut { arg index, item; 
		_BasicWrapPut;
		^this.primitiveFailed;
	}
	foldPut { arg index, item; 
		_BasicFoldPut;
		^this.primitiveFailed;
	}
	removeAt { arg index; 
		_BasicRemoveAt;
		^this.primitiveFailed;
	}
	takeAt { arg index; 
		_BasicTakeAt;
		^this.primitiveFailed;
	}
	
	indexOf { arg item;
		_ArrayIndexOf
		^this.primitiveFailed;
	}

	// see counterparts to these in Object 
	slotSize {
		^this.size;
	}
	slotAt { arg index;
		^this.at(index);
	}
	slotPut { arg index, value;
		^this.put(index, value);
	}
	slotKey { arg index;
		^index
	}
	slotIndex { arg key;
		^nil
	}
	getSlots {
		^this.copy
	}
	setSlots { arg array;
		this.overWrite(array)
	}
	
	atModify { arg index, function; function.value(this.at(index)) }
	atInc { arg index, inc=1; this.put(index, this.at(index)+inc); }
	atDec { arg index, dec=1; this.put(index, this.at(index)-dec); }

	isArray { ^true }
	asArray { ^this }
	*with { arg ... args; 
		var newColl;
		// answer an array of my class of the given arguments
		// the class Array has a simpler implementation
		newColl = this.new(args.size);
		args.do({ arg item, i; newColl.add(i, item) });
		^newColl
	}
	*series { arg size, start=0, step=1;
		var array;
		array = this.new(size);
		size.do({ arg i;
			array.add(start + (step * i));
		});
		^array
	}
	*geom { arg size, start, grow;
		var array;
		array = this.new(size);
		size.do({ arg i;
			array.add(start);
			start = start * grow;
		});
		^array
	}		

	copyRange { arg start, end; 
		// copies the fixed part of an object and the indexed slots
		// from start to end.
		_ObjectCopyRange;
		^this.primitiveFailed 
	}
	copySeries { arg first, second, last; 
		// copies elements from first to last, stepping by (second-first)
		// copySeries(3,5,11) copies elements 3,5,7,9,11
		// if second is nil then the step is 1.
		// copySeries(3,nil,11) copies elements 3,4,5,6,7,8,9,10,11
		_ObjectCopySeries;
		^this.primitiveFailed 
	}
	putSeries { arg first, second, last, value; 
		// puts value into array at indices defined by the series. see copySeries.
		_ArrayPutSeries;
		^this.primitiveFailed 
	}
	
	add { arg item; 
		// add item to end of array.
		// if the capacity is exceeded, this returns a new
		// ArrayedCollection.
		_ArrayAdd 
		^this.primitiveFailed; 
	}
	addAll { arg aCollection; 
		var array;
		_ArrayAddAll
		array = this;
		aCollection.asCollection.do({ arg item; array = array.add(item) }) ;
		^array
	}
	putEach { arg keys, values; 
		_ArrayPutEach
		^super.putEach(keys, values)
	}
	extend { arg size, item; 
		_ArrayExtend
		^this.primitiveFailed
	}
	insert { arg index, item; 
		// add item at specified index.
		// if the capacity is exceeded, this returns a new
		// ArrayedCollection.
		_ArrayInsert
		^this.primitiveFailed; 
	}
	addFirst { arg item; ^this.insert(0, item) }
	addIfNotNil { arg item; if(item.notNil,{ ^this.add(item) }) }
	pop { 
		// remove and return last item in array
		_ArrayPop 
		^nil; 
	}
	++ { arg anArray; 
		// concatenate two arrays of the same type
		// this primitive will handle all array element types
		_ArrayCat;
		// primitive fails if arrays are different types
		^super ++ anArray
	}
	overWrite { arg aCollection, pos=0;
		var array, grow;
		_ArrayOverwrite
		^this.primitiveFailed
		//array = this.growClear(pos + aCollection.size - this.size);
		//grow = pos + aCollection.size - this.size;
		//if (grow > 0, { array = this ++ this.class.newClear(grow); },{ array = this });
		//aCollection.do({ arg item, i; array.put(pos + i, item); });
		//^array
	}
	grow { arg sizeIncrease;
		// returns an array of sufficient capacity.
		// may return same object if it still has enough space or if sizeIncrease <= 0.
		_ArrayGrow 
		^this.primitiveFailed
	}
	growClear { arg sizeIncrease;
		// returns an array of sufficient capacity.
		// may return same object if it still has enough space or if sizeIncrease <= 0.
		// clears new space
		_ArrayGrowClear 
		^this.primitiveFailed
	}
	seriesFill { arg start, step;
		this.size.do({ arg i;
			this.put(i, start);
			start = start + step;
		});
	}

	fill { arg value; 
		_ArrayFill 
		^this.primitiveFailed
		/* replaced by primitive
		var i = 0, size;
		size = this.size;
		while ({ i < size }, {
			this.put(i, val);
			i = i + 1;
		});
		*/
	}
	do { arg function;
		// special byte codes inserted by compiler for this method
		var size, i=0;
		size = this.size;
		while ({ i < size }, {
			function.value(this.at(i), i);
			i = i + 1;
		})
	}
	reverseDo { arg function;
		// special byte codes inserted by compiler for this method
		var i=0, j=0;
		i = this.size - 1;
		while ({ i >= 0 },{ 
			function.value(this.at(i), j);
			i = i - 1;
			j = j + 1;
		})
	}
	reverse { 
		var i = 0, size1, halfsize;
		size1 = this.size - 1;
		halfsize = this.size div: 2;
		halfsize.do({ arg i;
			this.swap(i, size1 - i);
		});
	}
	windex {
		_ArrayWIndex
		^this.primitiveFailed 
		//		var r, sum = 0.0, index;
		//		r = 1.0.rand;
		//		this.detect({ arg weight, i;
		//			sum = sum + weight;
		//			if (sum >= r, {
		//				index = i;
		//				true;
		//			},{ false });
		//		});
		//		^index;
	}
	normalizeSum {
		_ArrayNormalizeSum
		^(this / this.sum)
	}
	asciiPlot {
		// draw the waveform down the page as asterisks
		var lo, hi, scale, pt;
		lo = this.minItem; 
		hi = this.maxItem;
		scale = 80 / (hi - lo);
		this.size.do { |i|
			pt = ((this[i] - lo) * scale).asInteger;
			pt.do({ " ".post; });
			"*\n".post;
		};
	}

	perfectShuffle {
		^this[(0 .. this.size div: 2 - 1).stutter + [0, this.size + 1 div: 2]]
	}

	// concepts borrowed from J programming language
	rank {
		// rank is the number of dimensions in a multidimensional array.
		// see also Object-rank
		// this assumes every element has the same rank
		^ 1 + this.first.rank
	}
	shape {
		// this assumes every element has the same shape
		^[this.size] ++ this[0].shape
	}
	reshape { arg ... shape;
		var result, size;
		size = shape.product;
		result = this.flat.wrapExtend(size);
		shape[1..].reverseDo {|n| result = result.clump(n) };
		^result
	}
	deepCollect { arg depth = 1, function;
		if (depth <= 0) {
			^function.value(this, 0)
		};
		depth = depth-1;
		^this.collect {|item| item.deepCollect(depth, function) }
	}
	reshapeLike { arg another, indexing=\wrapAt;
		var flat, index = 0;	
		flat = this.flat;
		^another.deepCollect(0x7FFFFFFF) {
			var item;
			item = flat.perform(indexing, index);
			index = index + 1;
			item;
		};
	}
	
	unbubble { arg depth=0, levels=1;
		if (depth <= 0) {
			// converts a size 1 array to the item.
			if (this.size > 1) { ^this };
			if (levels <= 1) { ^this[0] }
			^this[0].unbubble(depth, levels-1)
		};
		^this.collect {|item| item.unbubble(depth-1) } 
	}
	bubble { arg depth=0, levels=1;
		if (depth <= 0) { 
			if (levels <= 1) { ^[this] }
			^[this.bubble(depth,levels-1)]
		};
		^this.collect {|item| item.bubble(depth-1, levels) } 
	}
	
	slice { arg ... cuts;
		var firstCut, index, list;
		if (cuts.size == 0) { ^this.copy };

		firstCut = cuts[0];
		if (firstCut.isNil) { list = this.copy } { list = this[firstCut.asArray] };
		if (cuts.size == 1) {
			^list.unbubble
		}{
			cuts = cuts[1..];
			^list.collect {|item| item.slice(*cuts) }.unbubble
		}
	}
	*iota { arg ... sizes;
		^(0..sizes.product-1).reshape(*sizes)
	}
	*fill2D { arg rows, cols, function;
		var array;
		array = this.new(rows);
		rows.do{|row|
			var array2;
			array2 = this.new(cols);
			cols.do{|col|
				array2 = array2.add(function.(row, col))
			};
			array = array.add(array2);
		};
		^array
	}
	*fill3D { arg planes, rows, cols, function;
		var array;
		array = this.new(planes);
		planes.do{|plane|
			var array2;
			array2 = this.new(rows);
			rows.do{|row|
				var array3;
				array3 = this.new(cols);
				cols.do{|col|
					array3 = array3.add(function.(plane, row, col))
				};
				array2 = array2.add(array3);
			};
			array = array.add(array2);
		};
		^array
	}
}

RawArray : ArrayedCollection {
}

Int8Array[int8] : RawArray {
	unarchive {
		_Unarchive
		^this.primitiveFailed 
	}
}

Int16Array[int16] : RawArray {
}

Int32Array[int32] : RawArray {
}

FloatArray[float] : RawArray {
}

DoubleArray[double] : RawArray {
}

SymbolArray[symbol] : RawArray {
}

