

PatchIn {

	var <>nodeControl,<>connectedTo;
	
	*new  { arg nodeControl;
		^super.newCopyArgs(nodeControl)
	}
	*newByRate { arg rate,nodeControl;
		^this.perform(rate,nodeControl)
	}
	*audio { arg nodeControl;
		^AudioPatchIn(nodeControl)
	}
	*control { arg nodeControl;
		^ControlPatchIn(nodeControl)
	}
	*scalar { arg nodeControl;
		^ScalarPatchIn(nodeControl)
	}
	connectTo { arg patchOut;
		// break any previous ?
		// connectedTo = patchOut; //  patchOut does me
		patchOut.connectTo(this);
	}
	server { ^nodeControl.server }
	group { ^nodeControl.group }
}
	
AudioPatchIn : PatchIn {

	rate { ^\audio }
	readFromBus { arg bus;
		nodeControl.value = bus.index;
	}
	value_ { arg val;
		// the nodeControl indicates the audio bus number

		// to set the value of audio, would have to set up a bus and set the sample value
		thisMethod.notYetImplemented;
	}
	
}	
	
ControlPatchIn : AudioPatchIn {
	rate { ^\control }
	value_ { arg val;
		nodeControl.value = val;
	}
}

ScalarPatchIn : ControlPatchIn {
	rate { ^\scalar }
}



PatchOut {

	var <source,<group,<>bus;
	var <connectedTo;

	*new { arg source,group,bus;
		^this.perform(source.rate,source,group,bus)
	}
	*prNew {arg source,group,bus;
		 ^super.newCopyArgs(source,group.asGroup,bus).init;
	}
	*audio { arg source,group,bus;
		^AudioPatchOut.prNew(source, group,bus)
	}
	*control { arg source,group,bus;
		^ControlPatchOut.prNew(source, group,bus)
	}
	*scalar { arg source, group,bus;
		^ScalarPatchOut.prNew(source, group,bus)
	}
	connectTo { arg patchIn;
		connectedTo = connectedTo.add(patchIn);
		patchIn.connectedTo = this;
		this.perform(patchIn.rate,patchIn);// set value, bus etc.
	}
	free {
		// tell my connectedTo that i'm gone
	}
}

ControlPatchOut : PatchOut { // you are returned from a .kr play
	
	init {
		// nil : speakers
		// integer : that out
		// server : private on that server 
		bus = bus.asBus(this.rate,source.numChannels,this.server);
	}
	rate { ^\control }
	value { ^bus.index } //need some initialValue

	// private
	control { arg controlPatchIn;
		// TODO ! check if on same server

		// dont have to this unless changed
		controlPatchIn.readFromBus(bus)
	}
	audio { arg audioPatchIn;
		// K2A convertor needed
		// the patchIn owns the convertor ?
		// per input ?
		thisMethod.notYetImplemented;
	}
	scalar { arg scalarPatchIn;
		// polling of value not yet implemented on scserver
		// scalarPatchIn.value = bus.poll;
		thisMethod.notYetImplemented;
	}
	server { ^group.server }
	free {
		bus.free;
		bus = nil;
	}
}

AudioPatchOut : ControlPatchOut {
	rate { ^\audio }
	value { ^bus.index }
	audio { arg audioPatchIn;
		// check if on same server
		audioPatchIn.readFromBus(bus)
		// create mixer if needed
	}
	control { arg controlPatchIn;
		// A2K convertor needed ?
		// how else can i read from a .kr buss ?
		thisMethod.notYetImplemented;
	}
	scalar { arg scalarPatchIn;
		// polling of value not yet implemented on scserver
		// scalarPatchIn.value = bus.poll;
		thisMethod.notYetImplemented;
	}
}

ScalarPatchOut : PatchOut { 

	// floats,NumberEditors, numeric pattern players, midi, wacom
	// all respond to .value
	// things that are not ON the server
	
	var updater; // multiple outs, multiple updaters
	
	*new { arg source,bus; // TODO use this to get server...
		^this.prNew(source)
	}
	// no group needed
	*prNew {arg source,group,bus; ^super.newCopyArgs(source,group,bus).init }

	init {}
	rate { ^\scalar }
	value {
		//["initial ",source.value,source].postln;
	 	^source.value 
	 }
	audio { arg audioPatchIn;
		audioPatchIn.value = source.value;
	}
	control { arg controlPatchIn;
		controlPatchIn.value = source.value;
		updater = SimpleController(source)
					.put(\value,{
						controlPatchIn.value = source.value;
					});
	}
	scalar { arg scalarPatchIn;
		thisMethod.notYetImplemented;
	}
	free {
		updater.remove;
		updater = nil;
	}
}
