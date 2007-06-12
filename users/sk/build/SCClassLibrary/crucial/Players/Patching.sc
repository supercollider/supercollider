
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
	*stream { arg nodeControl;
		^ScalarPatchIn(nodeControl)
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
	value_ { arg val;
		// nothing to do 
	}
}



PatchOut {

	var <>source,<>group,<bus;
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
	*stream { arg source,group,bus;
		^ScalarPatchOut.prNew(source, group,bus)
	}		
	init {}
	connectTo { arg patchIn,needsValueSetNow=true;
		// am i already connected to this client ?
		if(connectedTo.isNil or: {connectedTo.includes(patchIn).not},{
			connectedTo = connectedTo.add(patchIn);
		});
		patchIn.connectedTo = this;
		this.perform(patchIn.rate,patchIn,needsValueSetNow);// set value, bus etc.
	}
	disconnect {
		("disconnect" + this).postln;
	}
	free {
		// tell my connectedTo that i'm gone
	}
	/*pause { arg requester;
		if(connectedTo.every({ arg cn; cn === requester }),{
			// then we can pause
		})
	}*/
	server { ^group.server }
	isPlaying { ^group.isPlaying }
	bus_ { arg b;
		bus = b.asBus;
		connectedTo.do({ arg pti;
			pti.readFromBus(bus);
		})
	}
	hasBus { ^bus.notNil and: {bus.index.notNil} }
		
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
		[this,scalarPatchIn,this.source]
			.debug("control -> scalar patch ? this,scalarPatchIn,this.source");
		thisMethod.notYetImplemented;
	}
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
		//no problem, an audio output into a control input is common
		// and no action needs to be taken
		//[controlPatchIn,this,"control -> audio"].insp;
	}
	scalar { arg scalarPatchIn;
		// polling of value not yet implemented on scserver
		// scalarPatchIn.value = bus.get;
		//[this,scalarPatchIn].insp("audio -> scalar not yet implemented");
		thisMethod.notYetImplemented;
	}
}

//AbstractScalarPatchOut
ScalarPatchOut : PatchOut { 

	// floats,NumberEditors, numeric pattern players, midi, wacom
	// things that are not ON the server

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
	}
	scalar { arg scalarPatchIn;
		//thisMethod.notYetImplemented;
	}
}

UpdatingScalarPatchOut : ScalarPatchOut {
	var enabled=false;
	*new { arg source,bus,enabled=true;
		^this.prNew(source,enabled)
	}
	*prNew {arg source,enabled; ^super.newCopyArgs(source).init(enabled) }

	init { arg enabled=true;
		if(enabled,{
			source.addDependant(this)
		})
	}
	audio { arg audioPatchIn,needsValueSetNow=false;
		if(needsValueSetNow,{
			audioPatchIn.value = source.value;
		});
		this.enable;
	}
	control { arg controlPatchIn,needsValueSetNow=false;
		if(needsValueSetNow,{
			controlPatchIn.value = source.value;
		});
		this.enable;
	}
	scalar { arg scalarPatchIn;
		this.enable;
	}
	enable { 
		if(enabled.not,{
			source.addDependant(this);
			enabled = true;
		})
	}
	update {
		connectedTo.do({ arg c; c.value = source.value })
	}
	free {
		source.removeDependant(this);
		enabled = false;
	}
}
