MultiLevelIdentityDictionary : Collection {		var <>dictionary;	*new {		^super.new.init	}	init {		dictionary = this.newInternalNode;	}		newInternalNode { ^IdentityDictionary.new }	at { arg ... args;		var item;		item = dictionary;		args.do({ arg name; 			item = item.at(name);			if (item.isNil, { ^nil });		});		^item	}		put { arg ... args;		var item, val, lastName;		val = args.pop;		lastName = args.pop;		item = dictionary;		args.do({ arg name; 			item = item.atFail(name, {				var newitem; 				newitem = this.newInternalNode;				item.put(name, newitem);				newitem			});		});		item.put(lastName, val);	}	create { arg ... args;		var item;		item = dictionary;		args.do({ arg name; 			item = item.atFail(name, {				var newitem; 				newitem = this.newInternalNode;				item.put(name, newitem);				newitem			});		});	}	removeAt { arg ... args;		var item, lastName;		lastName = args.pop;		item = dictionary;		args.do({ arg name; 			item = item.at(name); 			if (item.isNil, { ^nil });		});		item.removeAt(lastName);	}	do { arg function;		dictionary.do(function);	}	add { arg assn;		this.put(assn.key, assn.value);	}	remove { ^this.shouldNotImplement(thisMethod) }	removeFail { ^this.shouldNotImplement(thisMethod) }	}


Library : MultiLevelIdentityDictionary 
{
	classvar <global;
	
	*initClass {
		global = this.new;
	}

	*clear {
		global = this.new;
	}
	*at { arg ... args;
		^global.performList(\at, args);
	}

	*atList { arg args;
		^global.performList(\at,args)
	}
	*putList { arg args;
		^global.performList(\put,args)
	}
	
	*put { arg ... args;
		global.performList(\put,args);
	}
	*create { arg ... args;
		^global.performList(\create, args);
	}
	/*
	*localPut { arg ... args;
		^global.performList(\put, args);
	}
	*localCreate { arg ... args;
		^global.performList(\create, args);
	}
	*/
	
	*postTree {
		global.postTree
	}
	postTree {
		this.doPostTree(dictionary)		
	}
	doPostTree { arg obj,tabs=0;
		if(obj.isKindOf(IdentityDictionary),{
			"".postln;
			obj.keysValuesDo({ arg k,v;
				tabs.do({ Char.tab.post });
				k.post;
				": ".post;
				this.doPostTree(v,tabs + 1)
			});
		},{
			Char.tab.post;
			obj.asString.postln;
		})
	}
}

