
+ Object {

	guiClass { ^ObjectGui }

	gui { arg lay ... args; 
		^this.guiClass.new(this).performList(\gui,[lay] ++ args);
	}
	
	topGui { arg lay; 
		^this.guiClass.new(this).topGui(lay);
	}
	
	smallGui { arg lay;
		^this.guiClass.new(this).smallGui(lay);
	}
	
	insp { arg  ... args;
		Insp(this,args);
	}
	
}

+ Class {
	guiClass { ^ClassGui }
}

+ Method {
	guiClass { ^MethodGui }
}

