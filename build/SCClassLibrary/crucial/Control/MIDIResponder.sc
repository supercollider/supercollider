

MIDIResponder {
	classvar initialized = false,responders;
	var <>function;
	init {
		if(initialized.not,{ this.class.init });
		this.class.add(this);
	}
	remove {
		this.class.remove(this)
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

NoteOnResponder : MIDIResponder {
	*new { arg function;
		^super.new.function_(function).init
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		initialized = true;
		responders = [];
		MIDIIn.noteOn = { arg src, chan, num, veloc;
			responders.do({ arg r;
				r.function.value(num,veloc);
			});
		}
	}
	value { arg note,veloc;
		function.value(note,veloc)
	}
	*add { arg resp;
		responders = responders.add(resp);
	}
	*remove { arg resp;
		responders.remove(resp);
	}		
}

NoteOffResponder : NoteOnResponder {

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		initialized = true;
		responders = [];
		MIDIIn.noteOff = { arg src, chan, num, veloc;
			responders.do({ arg r;
				r.function.value(num,veloc);
			});
		}
	}
}

CCResponder : MIDIResponder {
	var <>num;
	
	*new { arg num,function;
		^super.new.num_(num).function_(function).init
	}
	value { arg val;
		function.value(val)
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
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		initialized = true;
		responders = Array.newClear(127);
		MIDIIn.control = { arg src,chan,num,val;
			responders.at(num).value(val);
		};
	}
}


/*
TouchResponder
BendResponder
PolyTouchResponder
*/

