TwoWayIdentityDictionary : Collection {
	var idToObj, objToID;

	*new {
		^super.new.init
	}

	add { |anAssociation|
		this.put(anAssociation.key, anAssociation.value)
	}
	put { |key, obj|
		idToObj.put(key, obj);
		objToID.put(obj, key)
	}

	remove { |obj|
		var key, out;
		key = this.getID(obj);
		out = idToObj.removeAt(key);
		objToID.removeAt(obj);
		^out
	}

	removeAt { |key|
		var obj = this.at(key);
		idToObj.removeAt(key);
		objToID.removeAt(obj)
	}

	do { |function|
		^idToObj.do(function)
	}

	at { |id|
		^idToObj.at(id)
	}

	getID { |obj|
		^objToID.at(obj)
	}

	storeItemsOn { |stream, itemsPerLine = 5|
		^idToObj.storeItemsOn(stream, itemsPerLine)
	}

	printItemsOn { |stream, itemsPerLine = 5|
		^idToObj.printItemsOn(stream, itemsPerLine)
	}


	// PRIVATE
	init {
		idToObj = IdentityDictionary.new;
		objToID = IdentityDictionary.new
	}
}

UniqueID {
	classvar <id = 1000;
	*initClass { id = 1000 }
	*next  { ^id = id + 1 }
}

ObjectTable : TwoWayIdentityDictionary {
	classvar <global;


	*new {
		^super.new
	}

	add { |obj|
		this.put(UniqueID.next, obj)
	}

	*initClass {
		global = this.new
	}
	*add { |obj|
		global.add(obj);
		^UniqueID.id
	}
	*put { |key, obj|
		global.put(key, obj)
	}
	*remove { |obj|
		global.remove(obj)
	}
	*at { |id|
		^global.at(id)
	}
	*getID { |obj|
		^global.getID(obj)
	}
	*objPerform { |id, selector ... args|
		var obj;
		obj = global.at(id);
		if(obj.notNil) {
			obj.performList(selector, args)
		}
	}
}
