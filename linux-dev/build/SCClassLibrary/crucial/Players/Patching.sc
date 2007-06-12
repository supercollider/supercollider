

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
//	connectTo { arg patchOut,needsValueSetNow=true;
//		// break any previous ?
//		patchOut.connectTo(this,needsValueSetNow);
//	}
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
		//[this,nodeControl].insp(thisMethod);
		nodeControl.value = val;
	}
}

ScalarPatchIn : ControlPatchIn {
	rate { ^\scalar }
	value_ { arg val;
		// nothing to do 
	}
}



PatchOut {

	var <source,<>group,<bus;
	var <connectedTo,<>patchOutsOfInputs;

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
	connectTo { arg patchIn,needsValueSetNow=true;
		connectedTo = connectedTo.add(patchIn);
		patchIn.connectedTo = this;
		this.perform(patchIn.rate,patchIn,needsValueSetNow);// set value, bus etc.
	}
	free {
		// tell my connectedTo that i'm gone
	}
//	pause { arg requester;
//		if(connectedTo.every({ arg cn; cn === requester }),{
//			// then we can pause
//		})
//	}
	bus_ { arg b;
		bus = b.asBus;
		connectedTo.do({ arg pti;
			pti.readFromBus(bus);
		})
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
	synthArg { ^bus.index } //need some initialValue

	// private
	control { arg controlPatchIn,needsValueSetNow;
		if(needsValueSetNow,{
			// TODO ! check if on same server
			// dont have to this unless changed
			controlPatchIn.readFromBus(bus)
		})
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
		//["notYetImplemented",this,scalarPatchIn].insp("scalar to scalar patch ?");
		thisMethod.notYetImplemented;
	}
	server { ^group.server }
	free {
		// PlayerMixer has multiple patchOuts sharing the same bus
		if(bus.notNil and: {bus.index.notNil},{
			bus.free;
		});
		bus = nil;
	}
}

AudioPatchOut : ControlPatchOut {
	rate { ^\audio }
	synthArg { ^bus.index }
	audio { arg audioPatchIn,needsValueSetNow;
		if(needsValueSetNow,{
			// check if on same server
			audioPatchIn.readFromBus(bus)
			// create mixer if needed
		});
	}
	control { arg controlPatchIn,needsValueSetNow;
		// A2K convertor needed ?
		// how else can i get a .kr input to read from an .ar buss ?
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

	// things that are not ON the server
	
	var <>updater; // multiple outs, multiple updaters
	
	*new { arg source,bus; // TODO use this to get server...
		^this.prNew(source)
	}
	// no group needed
	*prNew {arg source,group,bus; ^super.newCopyArgs(source,group,bus).init }

	init {}
	rate { ^\scalar }
	synthArg {
	 	^source.synthArg
	 }
	audio { arg audioPatchIn,needsValueSetNow=false;
		if(needsValueSetNow,{
			audioPatchIn.value = source.value;
		})
	}
	control { arg controlPatchIn,needsValueSetNow=false;
		if(needsValueSetNow,{
			controlPatchIn.value = source.value;
		});
		//updater moved to the object
	}
	scalar { arg scalarPatchIn;
		//thisMethod.notYetImplemented;
	}
	free {
		updater.remove;
		updater = nil;
	}
}
