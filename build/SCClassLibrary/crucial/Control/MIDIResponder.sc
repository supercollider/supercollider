
	
CCResponder {
	classvar initialized = false,responders;

	var <num,<function;
	
	*new { arg num,function;
		^super.newCopyArgs(num,function).init
	}
	init {
		if(initialized.not,{ this.class.init });
		this.class.add(this);
	}
	remove {
		this.class.remove(this)
	}
	value { arg val;
		function.value(val)
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		responders = Array.newClear(127);
		MIDIIn.control = this;
		initialized = true;
	}
	*add { arg resp;
		var old;
		old = responders.at(resp.num);
		if(old.isNil,{
			responders.put(resp.num,resp);
		},{
			if(old.isKindOf(ResponderArray).not,{
				responders.put(resp.num,ResponderArray.new.add(old).add(resp))
			},{
				old.add(resp)
			})
		})
	}
	*removeAll { this.init }
	*remove { arg resp;
		responders.do({ arg r,i;
			if(r === resp,{
				responders.put(i,nil)
			},{
				if(r.isKindOf(ResponderArray),{
					if( r.remove(resp) == 0,{
						responders.put(i,nil); // all cleared
					})
				})
			})
		})
	}
	*value { arg src,chan,num,val;
		responders.at(num).value(val);
	}
}

ResponderArray {
	var array;
	add { arg thing;
		array = array.add(thing)
	}
	value { arg val;
		array.do({ arg resp; resp.function.value(val) })
	}
	remove { arg resp;
		array.remove(resp);
		^array.size
	}
	
}


