
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
		^KrNumberEditor.new(this.constrain(val ? this.default),this); 
	}

	rate { ^\control }

}




