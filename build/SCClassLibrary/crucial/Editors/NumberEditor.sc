
Editor {
	var  <>action, // { arg value,theEditor; }
		<>value, <patchOut;
	
	guiClass { ^ObjectGui }

	storeOn { arg stream;
		value.storeOn(stream)
	}

	next { ^this.value }// Object would return this
	poll { ^value }
	
	setPatchOut { arg po; patchOut = po }

	editWithCallback { arg callback;
		ModalDialog({ arg layout;
			this.gui(layout);
		},{
			callback.value(value);
		})
	}
	
	prepareToBundle { arg group,bundle;
		// i need this now ?
		if(patchOut.isNil,{ // private out
			//patchOut = UpdatingScalarPatchOut(this,enabled: false);
			patchOut = ScalarPatchOut(this);
		});
	}
	synthArg { ^this.poll }
	instrArgFromControl { arg control;
		^control
	}
}

KrNumberEditor : Editor {
	
	var <spec,lag=0.05;
	
	*new { arg value=1.0,spec='amp';
		^super.new.init(value,spec)
	}
	init { arg val,aspec;
		spec = aspec.asSpec ?? {ControlSpec.new};
		this.value_(spec.constrain(val));
	}
//	value_ { arg val;
//		value = val;
//		// server support
//		// could also do it with a dependant
//		//if(patchIn.notNil,{ patchIn.value = value });
//		//this.changed(\value,changer);
//	}
	activeValue_ { arg val;
		this.value_(val);
		action.value(value);
	}
	spec_ { arg aspec;
		spec = aspec.asSpec;
		value = spec.constrain(value);
		this.changed(\spec);
	}
	canDoSpec { arg aspec; ^aspec.isKindOf(ControlSpec) }

	addToSynthDef {  arg synthDef,name;
		synthDef.addKr(name,this.synthArg);
	}
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		//should request a LagControl
		if(lag > 0.0,{
			^Lag.kr(control,lag)
		},{
			^control
		})
	}
	didSpawn { arg patchIn,synthi;
		//patchOut.enable;
		patchOut.connectTo(patchIn,false);
		// am i already connected to this client ?
		if(this.dependants.includes(patchOut.updater).not,{
			patchOut.updater = 
				Updater(this,{
					patchIn.value = value;
				});
		});
	}
	free {
		//patchOut.free;
		if(patchOut.notNil and: {patchOut.updater.notNil},{
			patchOut.updater.remove;
			patchOut.updater = nil;
		});
	}
	
	guiClass { ^NumberEditorGui }

}


// for controls that won't talk to the server
NumberEditor : KrNumberEditor { 

	addToSynthDef { arg synthDef,name;
		synthDef.addInstrOnlyArg(name,this.synthArg)
	}
	instrArgFromControl { arg control;
		^value
	}
	instrArgRate { ^\scalar }

}
IntegerEditor : NumberEditor {

	value_ { arg val,changer;
		value = val.asInteger;
		this.changed(\value,changer);
	}
}



BooleanEditor : NumberEditor {

	*new { arg val=false;
		^super.new.value_(val)
	}
	// value returns true/false

	guiClass { ^BooleanEditorGui }
}

