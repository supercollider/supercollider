
Editor {
	var  <>action, // { arg value,theEditor; }
		<>value;
	
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

NumberEditor : Editor {
	
	var <spec;
	
	*new { arg value=1.0,spec='amp';
		^super.new.init(value,spec)
	}
	init { arg val,aspec;
		spec = aspec.asSpec;
		value = spec.constrain(val);
	}
	value_ { arg val;
		value = val; //spec.constrain(val);
		// server support
		// could also do it with a dependant
		//if(patchIn.notNil,{ patchIn.value = value });
		this.changed(\value);
	}
	activeValue_ { arg val;
		this.value_(val);
		action.value(val);
	}
	spec_ { arg aspec;
		spec = aspec.asSpec;
		value = spec.constrain(value);
		this.changed(\spec);
	}
	canDoSpec { arg aspec; ^aspec.isKindOf(ControlSpec) }
	//kr { arg lag=0.05; ^Plug.kr({value},lag) }

	play { arg group,atTime;
		^this.spawn(atTime);
	}
	prepareForPlay {}
	readyForPlay { ^true }
	spawn { arg atTime;
		^ScalarPatchOut.new(this)
	}
	initDefArg { ^value }

	guiClass { ^NumberEditorGui }

}


/* obsolete

	KrNumberEditor
		optimised for change
		even the gui talks directly


KrNumberEditor : NumberEditor { 

	var <>lag;

	*new { arg value=1.0,spec='amp',lag=0.05;
		^super.new(value,spec).lag_(lag)
	}

	value {
		^this.kr(lag)
	}	
	
}


*/


BooleanEditor : NumberEditor {

	*new { arg val=false;
		^super.new.value_(val)
	}
	// value returns true/false
	// kr returns 1/0
	//kr { arg lag=0.05; ^Plug.kr({value.binaryValue},lag) }

	guiClass { ^BooleanEditorGui }
}

