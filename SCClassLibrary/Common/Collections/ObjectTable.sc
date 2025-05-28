TwoWayIdentityDictionary : Collection {
	var idToObj, objToID;

	*new {
		^super.new.init;
	}

	add { arg anAssociation;
		this.put(anAssociation.key, anAssociation.value);
	}
	put { arg key, obj;
		idToObj.put(key, obj);
		objToID.put(obj, key);
	}

	remove { arg obj;
		var key, out;
		key = this.getID(obj);
		out = idToObj.removeAt(key);
		objToID.removeAt(obj);
		^out
	}

	removeAt { arg key;
		var obj = this.at(key);
		idToObj.removeAt(key);
		objToID.removeAt(obj);
	}

	do { arg function;
		^idToObj.do(function);
	}

	at { arg id;
		^idToObj.at(id);
	}

	getID { arg obj;
		^objToID.at(obj);
	}

	storeItemsOn { arg stream, itemsPerLine = 5;
		^idToObj.storeItemsOn(stream, itemsPerLine)
	}

	printItemsOn { arg stream, itemsPerLine = 5;
		^idToObj.printItemsOn(stream, itemsPerLine)
	}


	// PRIVATE
	init {
		idToObj = IdentityDictionary.new;
		objToID = IdentityDictionary.new;
	}
}

UniqueID {
	classvar <id=1000;
	*initClass { id = 1000; }
	*next  { ^id = id + 1; }
}

ObjectTable : TwoWayIdentityDictionary {
	classvar <global;


	*new {
		^super.new;
	}

	add { arg obj;
		this.put(UniqueID.next, obj);
	}

	*initClass {
		global = this.new;
	}
	*add { arg obj;
		global.add(obj);
		^UniqueID.id
	}
	*put { arg key, obj;
		global.put(key, obj);
	}
	*remove { arg obj;
		global.remove(obj);
	}
	*at { arg id;
		^global.at(id);
	}
	*getID { arg obj;
		^global.getID(obj);
	}
	*objPerform { arg id, selector ... args;
		var obj;
		obj = global.at(id);
		if (obj.notNil, {
			obj.performList(selector, args);
		});
	}
}
