MultiLevelIdentityDictionary : Collection {		var <>dictionary;	*new {		^super.new.init	}	init {		dictionary = this.newInternalNode;	}		newInternalNode { ^IdentityDictionary.new }	at { arg ... args;		var item;		item = dictionary;		args.do({ arg name; 			item = item.at(name);			if (item.isNil, { ^nil });		});		^item	}		put { arg ... args;		var item, val, lastName;		val = args.pop;		lastName = args.pop;		item = dictionary;		args.do({ arg name; 			item = item.atFail(name, {				var newitem; 				newitem = this.newInternalNode;				item.put(name, newitem);				newitem			});		});		item.put(lastName, val);	}
	create { arg ... args;
		var item;
		item = dictionary;
		args.do({ arg name; 
			item = item.atFail(name, {
				var newitem; 
				newitem = this.newInternalNode;
				item.put(name, newitem);
				newitem
			});
		});
	}	
	
	choose { arg ... start;
		var item;
		if(start.isEmpty,{
			item = dictionary;
		},{
			item = this.performList(\at,start);
			if(item.isNil,{
				("Library-choose start address not found:" + start).die;
			});
		});
		^this.prChooseFrom(item);
	}
	putTree { arg ... items;
		this.prPutTree([],items)
	}
	postTree { arg obj,tabs=0;
		if(obj.isNil,{ obj = dictionary });
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
	do { arg function;
		dictionary.do(function);
	}
	removeAt { arg ... args;
		var item, lastName;
		lastName = args.pop;
		item = dictionary;
		args.do({ arg name; 
			item = item.at(name); 
			if (item.isNil, { ^nil });
		});
		item.removeAt(lastName);
	}

	
	//private	add { arg assn;		this.put(assn.key, assn.value);	}	remove { ^this.shouldNotImplement(thisMethod) }	removeFail { ^this.shouldNotImplement(thisMethod) }
	
	prChooseFrom { arg dict;
		var item;
		item = dict.choose;
		if(item.isKindOf(IdentityDictionary),{
			^this.prChooseFrom(item);
		},{
			^item
		})
	}
	prPutTree { arg keys,items;
		forBy(0,items.size - 1,2,{ arg i;
			var key,item;
			key = items.at(i);
			item = items.at(i + 1);
			if(item.isKindOf(Function),{
				this.performList(\put,keys ++ [key,item]);
			},{
				//array
				this.prPutTree(keys ++ [key],item);
			})
		})
	}
	leaves { arg startAt;
		if(startAt.isNil,{
			startAt = dictionary;
		},{
			startAt = this.performList(\at,startAt);
		});
		^this.prNestedValuesFromDict(startAt);
	}
	prNestedValuesFromDict { arg dict;
		^dict.values.collect({ arg thing;
			if(thing.isKindOf(IdentityDictionary),{
				this.prNestedValuesFromDict(thing)
			},{
				thing
			})
		})
	}}


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

	*postTree {
		global.postTree
	}

}

