
// see MIDIResponder help for all classes on this page

// rather than hog the midi inputs here, make a global hog for all piglets to suckle from

MIDIResponder {
	var <>function;
	init { arg install;
		if(this.class.initialized.not,{ this.class.init });
		if(install,{this.class.add(this);});
	}
	remove {
		this.class.remove(this)
	}
	*removeAll { this.init }
}

ResponderArray {
	var <>array;
	*new { arg array;
		^super.new.array_(array)
	}
	add { arg thing;
		array = array.add(thing)
	}
	value { arg ... vals;
		array.do({ arg resp; resp.performList(\value,vals) })
	}
	remove { arg resp;
		array.remove(resp);
		^array.size
	}
}

NoteOnResponder : MIDIResponder {
	classvar <norinit = false,<nonr;

	*new { arg function,install=true;
		^super.new.function_(function).init(install)
	}
	*initialized { ^norinit }
	*responders { ^nonr }
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		norinit = true;
		nonr = [];
		MIDIIn.noteOn = { arg src, chan, num, veloc;
			nonr.do({ arg r;
				r.value(src,chan,num,veloc);
			});
		}
	}
	value { arg src,chan,note,veloc;
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
				r.value(src,chan,num,veloc);
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
	
	*new { arg num,function,install=true;
		^super.new.num_(num).function_(function).init(install)
	}
	*initialized { ^ccinit }
	*responders { ^ccr }
	value { arg src,chan,ccnum,val;
		if(num == ccnum,{ function.value(val) })
	}
	*add { arg resp;
		if(this.initialized.not,{ this.init });
		ccr = ccr.add(resp);
	}
	*remove { arg resp;
		ccr.remove(resp);
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		ccinit = true;
		ccr = Array.newClear(127);
		MIDIIn.control = { arg src,chan,num,val;
			ccr.do({ arg ccr; ccr.value(src,chan,num,val) });
		};
	}
}



/*
TouchResponder
BendResponder
PolyTouchResponder
*/

