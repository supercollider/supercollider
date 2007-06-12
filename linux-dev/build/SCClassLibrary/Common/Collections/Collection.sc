Collection { 
	*newFrom { arg aCollection;
		var newCollection;
		newCollection = this.new(aCollection.size);
		aCollection.do({ arg item, i; newCollection.add(item) });
		^newCollection
	}
	*with { arg ... args; 
		var newColl;
		// answer an array of my class of the given arguments
		// the class Array has a simpler implementation
		newColl = this.new(args.size);
		newColl.addAll(args);
		^newColl
	}
	*fill { arg size, function;
		var obj;
		obj = this.new(size);
		size.do({ arg i;
			obj.add(function.value(i));
		});
		^obj
	}	

	@ { arg index; ^this.at(index) }
	
	== { arg aCollection;
		if (aCollection.class != this.class, { ^false });
		if (this.size != aCollection.size, { ^false });
		this.do({ arg item, i;
			if (item != aCollection.at(i), { ^false });
		});
		^true
	}
	hash { 
		var hash;
		hash = this.class.hash;
		this.do({ arg item;
			hash = hash bitXor: item.hash;
		});
		^hash
	}
	
	species { ^Array }
	do { ^this.subclassResponsibility(thisMethod) }
	size { 
		// this is the slow way. Most collections have a faster way.
		var tally = 0;
		this.do({ tally = tally + 1 });
		^tally
	}
	
	isEmpty { ^this.size == 0 }
	notEmpty { ^this.size > 0 }
	asCollection { ^this }
	
	add { ^this.subclassResponsibility(thisMethod) }
	addAll { arg aCollection; aCollection.asCollection.do({ arg item; this.add(item) }) }
	remove { ^this.subclassResponsibility(thisMethod) }
	removeAll { arg list; list.do({ arg item; this.remove(item) }) }
	removeAllSuchThat { arg function; 
		var removedItems, copy;
		removedItems = this.species.new;
		copy = this.copy;
		copy.do({ arg item, i;
			if ( function.value(item, i), {
				this.remove(item);
				removedItems.add(item);
			})
		});
		^removedItems
	}
		
	includes { arg item1; 
		this.do({ arg item2; if (item1 === item2, {^true}) });
		^false
	}
	includesAny { arg aCollection;
		aCollection.do({ arg item; if (this.includes(item), {^true}) });
		^false
	}
	includesAll { arg aCollection;
		aCollection.do({ arg item; if (this.includes(item).not, {^false}) });
		^true 
	}
	
	collect { arg function;
		^this.collectAs(function, this.species);
	}
	select { arg function;
		^this.selectAs(function, this.species);
	}
	reject { arg function;
		^this.rejectAs(function, this.species);
	}
	collectAs { arg function, class;
		var i, res;
		res = class.new(this.size);
		this.do({ arg elem, i; res.add(function.value(elem, i)) })
		^res;
	}
	selectAs { arg function, class;
		var i, res;
		res = class.new(this.size);
		this.do({ arg elem, i; if (function.value(elem, i), { res.add(elem) }) })
		^res;
	}
	rejectAs { arg function, class;
		var i, res;
		res = class.new(this.size);
		this.do({ arg elem, i; if (function.value(elem, i).not, {res.add(elem)}) })
		^res;
	}
	detect { arg function;
		this.do({ arg elem, i; if (function.value(elem, i), { ^elem }) })
		^nil;
	}
	detectIndex { arg function;
		this.do({ arg elem, i; if (function.value(elem, i), { ^i }) })
		^nil;
	}
	lastForWhich { arg function;
		var prev;
		this.do({ arg elem, i; 
			if (function.value(elem, i), { 
				prev = elem;
			},{
				^prev
			}) 
		});
		^prev
	}
	lastIndexForWhich { arg function;
		var prev;
		this.do({ arg elem, i; 
			if (function.value(elem, i), { 
				prev = i;
			},{
				^prev
			}) 
		});
		^prev
	}

	inject { arg thisValue, function;
		var nextValue;
		nextValue = thisValue;
		this.do({ arg item, i; 
			nextValue = function.value(nextValue, item, i);
		});
		^nextValue
	}
	count { arg function;
		var sum = 0;
		this.do({ arg elem, i; if (function.value(elem, i), { sum=sum+1 }) })
		^sum;
	}
	occurencesOf { arg obj;
		var sum = 0;
		this.do({ arg elem; if (elem == obj, { sum=sum+1 }) })
		^sum;
	}
	any { arg function;
		this.do({ arg elem, i; if (function.value(elem, i), { ^true }) })
		^false;
	}
	every { arg function;
		this.do({ arg elem, i; if (function.value(elem, i).not, { ^false }) })
		^true;
	}
	sum { arg function;
		var i, sum = 0;
		if (function.isNil, { // optimized version if no function
			this.do({ arg elem; sum = sum + elem; })
		},{
			this.do({ arg elem, i; sum = sum + function.value(elem, i); })
		})
		^sum;
	}
	sumabs {  // sum of the absolute values - used to convert Mikael Laursen's rhythm lists.
		var sum = 0;
		this.do({ arg elem; 
			if (elem.isSequenceableCollection, { elem = elem.at(0) });
			sum = sum + elem.abs; 
		})
		^sum;
	}
	
	maxItem { arg function;
		var maxValue, maxElement;
		if (function.isNil, { // optimized version if no function
			this.do({ arg elem; 
				if (maxElement.isNil, {
					maxElement = elem; 
				},{
					if (elem > maxElement, {
						maxElement = elem; 
					})
				})		
			})
			^maxElement;
		},{
			this.do({ arg elem, i; var val;
				if (maxValue.isNil, {
					maxValue = function.value(elem, i);
					maxElement = elem; 
				},{ 
					val = function.value(elem, i);
					if (val > maxValue, {
						maxValue = val;
						maxElement = elem; 
					})
				})		
			})
			^maxElement;
		})
	}
	minItem { arg function;
		var minValue, minElement;
		if (function.isNil, { // optimized version if no function
			this.do({ arg elem, i; 
				if (minElement.isNil, {
					minElement = elem; 
				},{
					if (elem < minElement, {
						minElement = elem; 
					})
				})		
			});
			^minElement;
		},{
			this.do({ arg elem, i; var val;
				if (minValue.isNil, {
					minValue = function.value(elem, i);
					minElement = elem; 
				},{ 
					val = function.value(elem, i);
					if (val < minValue, {
						minValue = val;
						minElement = elem; 
					})
				})		
			})
			^minElement;
		})
	}
	maxValue { arg function;//	// must supply a function
		var maxValue, maxElement;
		this.do({ arg elem, i; 
				var val;
				if (maxValue.isNil, {
					maxValue = function.value(elem, i);
					maxElement = elem; 
				},{ 
					val = function.value(elem, i);
					if (val > maxValue, {
						maxValue = val;
						maxElement = elem; 
					})
				})		
		});
		^maxValue;
	}
	minValue { arg function;
		var maxValue, maxElement;
		this.do({ arg elem, i; var val;
				if (maxValue.isNil, {
					maxValue = function.value(elem, i);
					maxElement = elem; 
				},{ 
					val = function.value(elem, i);
					if (val > maxValue, {
						maxValue = val;
						maxElement = elem; 
					})
				})		
		});
		^maxValue;
	}
	
	// set theory methods by Fredrik Olofsson <f@fredrikolofsson.com>
	// union - the set of all elements that are either in A or in B or in both
	union { arg aCollection; 
		^if(aCollection.size > this.size,{
			aCollection.reject({ arg item; this.includes(item) }).addAll(this) 
		},{
			this.reject({ arg item; 
				aCollection.includes(item) 
			}).addAll(aCollection) 
		})
	}
	//   in both sets A and B
	intersection { arg aCollection; 
		^this.select({ arg item; aCollection.includes(item) }) 
	}
	//  in A but not in B
	difference { arg aCollection; ^this.removeAll(aCollection) }
	//  in either A or B but not in both
	symmetricDifference { arg aCollection; 
		var c; 
		c=this.copy; 
		this.removeAll(aCollection); 
		aCollection.removeAll(c); 
		^this.addAll(aCollection) 
	}
	//  true if every element in A is also contained in B
	isSubset { arg aCollection; ^aCollection.includesAll(this) }



	asArray { ^Array.new(this.size).addAll(this); }
	asBag { ^Bag.new(this.size).addAll(this); }
	asList { ^List.new(this.size).addAll(this); }
	asSet { ^Set.new(this.size).addAll(this); }
	asSortedList { ^SortedList.new(this.size).addAll(this); }
	
	printAll { this.do({ arg item; item.postln; }); } // convenience method
	dumpAll { this.do({ arg item; item.dump; }); } // convenience method
	
	printOn { arg stream;
		if (stream.atLimit, { ^this });
		stream << this.class.name << "[ " ;
		this.printItemsOn(stream);
		stream << " ]" ;
	}
	storeOn { arg stream;
		if (stream.atLimit, { ^this });
		stream << this.class.name << "[ " ;
		this.storeItemsOn(stream);
		stream << " ]" ;
	}
	storeItemsOn { arg stream;
		this.do({ arg item, i;
			if (stream.atLimit, { ^this });
			if (i != 0, { stream.comma.space; });
			item.storeOn(stream);
		});
	}
	printItemsOn { arg stream;
		this.do({ arg item, i;
			if (stream.atLimit, { ^this });
			if (i != 0, { stream.comma.space; });
			item.printOn(stream);
		});
	}
	
	// Synth support
	
	writeDef { arg file;
		file.putString("SCgf");
		file.putInt32(0); // file version
		file.putInt16(this.size); // number of defs in file.
		
		this.do({ arg item; item.writeDef(file); });
	}

	// graphical support
	draw { arg args;
		this.do({ arg item; item.draw(args); });
	}
	layout { arg bounds;
		this.do({ arg item; item.layout(bounds); });
	}
}
