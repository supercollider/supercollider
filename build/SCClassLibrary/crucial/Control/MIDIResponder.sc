
// see MIDIResponder help for all classes on this page

MIDIResponder {
	var <>function;
	init {
		if(this.class.initialized.not,{ this.class.init });
		this.class.add(this);
	}
	remove {
		this.class.remove(this)
	}
	*removeAll { this.init }
	/* *remove { arg resp;
		this.responders.do({ arg r,i;
			if(r === resp,{
				this.responders.put(i,nil)
			},{
				if(r.isKindOf(ResponderArray),{
					if(r.remove(resp) == 0,{
						this.responders.put(i,nil); // all cleared
					})
				})
			})
		})
	} */
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
	classvar <norinit = false,<nonr;

	*new { arg function;
		^super.new.function_(function).init
	}
	*initialized { ^norinit }
	*responders { ^nonr }
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		norinit = true;
		nonr = [];
		MIDIIn.noteOn = { arg src, chan, num, veloc;
			nonr.do({ arg r;
				r.function.value(num,veloc);
			});
		}
	}
	value { arg note,veloc;
		function.value(note,veloc)
	}
	*add { arg resp;
		nonr = nonr.add(resp);
	}
	*remove { arg resp;
		nonr.remove(resp);
	}		
}

NoteOffResponder : NoteOnResponder {
	classvar <noffinit = false,<noffr;

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		noffinit = true;
		noffr = [];
		MIDIIn.noteOff = { arg src, chan, num, veloc;
			noffr.do({ arg r;
				r.function.value(num,veloc);
			});
		}
	}
	*initialized { ^noffinit }
	*responders { ^noffr }

	*add { arg resp;
		noffr = noffr.add(resp);
	}
	*remove { arg resp;
		noffr.remove(resp);
	}
}

CCResponder : MIDIResponder {
	classvar <ccinit = false,<ccr;

	var <>num;
	
	*new { arg num,function;
		^super.new.num_(num).function_(function).init
	}
	*initialized { ^ccinit }
	*responders { ^ccr }
	value { arg val;
		function.value(val)
	}
	*add { arg resp;
		var old;
		old = ccr.at(resp.num);
		if(old.isNil,{
			ccr.put(resp.num,resp);
		},{
			if(old.isKindOf(ResponderArray).not,{
				ccr.put(resp.num,ResponderArray.new.add(old).add(resp))
			},{
				old.add(resp)
			})
		})
	}
	*remove { arg resp;
		var rar;
		rar = ccr.at(resp.num);
		if(rar.isKindOf(ResponderArray),{
			rar.remove(resp)
		},{
			ccr.put(resp.num,nil);
		})
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		ccinit = true;
		ccr = Array.newClear(127);
		MIDIIn.control = { arg src,chan,num,val;
			ccr.at(num).value(val);
		};
	}
}


/*
TouchResponder
BendResponder
PolyTouchResponder
*/

