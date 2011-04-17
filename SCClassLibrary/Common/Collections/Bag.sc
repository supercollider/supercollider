Bag : Collection {
	var <contents;

	*new { arg n=4;
		^super.new.setDictionary(n)
	}
	// testing
	includes { arg item;
		^contents.includesKey(item)
	}
	// adding
	add { arg item, count=1;
		if ( this.includes(item), {
			contents.put(item, contents.at(item) + count);
		},{
			contents.put(item, count);
		});
	}
	remove { arg item, count=1;
		var newCount;
		if ( this.includes(item), {
			newCount = contents.at(item) - count;
			if (newCount <= 0, {
				contents.removeAt(item);
			},{
				contents.put(item, newCount);
			});
		});
	}

	// accessing
	at { ^this.shouldNotImplement(thisMethod) }
	atFail { ^this.shouldNotImplement(thisMethod) }
	put { ^this.shouldNotImplement(thisMethod) }

		// get something from the bag without removing it
		// the item is the dictionary's key
	choose {
		var index, key, array;
		if( this.isEmpty, { ^nil }); // empty dictionary
		array = contents.array;
		while({
			index = (array.size >> 1).rand << 1; // generate an even index.
			array.at(index).isNil;			  // key is at even index.
		});
		// return the first non-nil key
		^array.at(index);
	}

	wchoose {
		var	items = Array(contents.size), counts = Array(contents.size);
		contents.keysValuesDo({ |item, count|
			items.add(item);
			counts.add(count);
		});
		^items[counts.normalizeSum.windex]
	}

	take {
		var	result = this.choose;
		this.remove(result);
		^result
	}

	// enumerating
	do { arg function;
		var j = 0;
		contents.associationsDo({ arg assn;
			 assn.value.do({
			 	function.value(assn.key, j);
			 	j = j + 1;
			 })
		});
	}
	countsDo { arg function;
		var j = 0;
		contents.associationsDo({ arg assn;
			function.value(assn.key,assn.value,j);
			 j = j + 1;
		});
	}
	itemCount { arg item;
		^(contents.at(item) ? 0)
	}

	// PRIVATE IMPLEMENTATION
	setDictionary { arg n;
		contents = Dictionary.new(n)
	}

	asBag { ^this }
}

IdentityBag : Bag
{
	// PRIVATE IMPLEMENTATION
	setDictionary { arg n;
		contents = IdentityDictionary.new(n)
	}
}
