
+ Nil {
	asSpec {
		^ControlSpec.new;
	}
}

+ Spec {
	rate { ^\scalar }
}


+ ControlSpec 	{

	defaultControl { arg val=0.0; 
		^NumberEditor.new(this.constrain(val ? this.default),this); 
	}

	rate { ^\control }


}

+ StaticSpec { // how do i stop the attempt to set a control ?
	defaultControl { arg val=0.0; 
		^NumberEditor.new(this.constrain(val ? this.default),this) 
	}
}

+ EnvSpec {
	defaultControl {
		^EnvEditor.new(Env.adsr)
	}
}

+ AudioSpec {
	defaultControl{ ^Ar(0.0) // silence
	}
}

+ TrigSpec {
	defaultControl {	
		^SimpleTrigger.new
	}
}

