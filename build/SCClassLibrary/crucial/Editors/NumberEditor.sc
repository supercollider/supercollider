
Editor {
	var  <>action, // { arg value,theEditor; }
		<>value, <>patchOut;
	
	guiClass { ^ObjectGui }

	storeOn { arg stream;
		value.storeOn(stream)
	}

	next { ^this.value }// Object would return this
	poll { ^value }
	
//	editWithCallback { arg callback;
//		ModalDialog({ arg layout;
//			this.gui(layout);
//		},{
//			callback.value(value);
//		})
//	}
}

KrNumberEditor : Editor {
	
	var <spec;
	
	*new { arg value=1.0,spec='amp';
		^super.new.init(value,spec)
	}
	init { arg val,aspec;
		spec = aspec.asSpec ?? {ControlSpec.new};
		this.value_(spec.constrain(val));
	}
	value_ { arg val,changer;
		value = val; //spec.constrain(val);
		// server support
		// could also do it with a dependant
		//if(patchIn.notNil,{ patchIn.value = value });
		//[this,value,changer,this.dependants].insp("number editor value_");
		this.changed(\value,changer);
	}
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
	//kr { arg lag=0.05; ^Plug.kr({value},lag) }


	addToSynthDef {  arg synthDef,name;
		synthDef.addKr(name,this.synthArg);
	}
	instrArgRate { ^\control }

	didSpawn { arg patchIn,synthi;
		patchOut.connectTo(patchIn,false);

		// am i already connected to this client ?
		if(this.dependants.includes(patchOut.updater).not,{
			patchOut.updater = 
				SimpleController(this)
						.put(\value,{
							patchIn.value = value;
						});
		});
	}
	free {
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
	// kr returns 1/0
	//kr { arg lag=0.05; ^Plug.kr({value.binaryValue},lag) }

	guiClass { ^BooleanEditorGui }
}

