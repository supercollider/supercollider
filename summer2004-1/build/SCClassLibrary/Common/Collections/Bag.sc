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
	
	// PRIVATE IMPLEMENTATION
	setDictionary { arg n;
		contents = Dictionary.new(n)
	}
}

IdentityBag : Bag
{
	// PRIVATE IMPLEMENTATION
	setDictionary { arg n;
		contents = IdentityDictionary.new(n)
	}
}

