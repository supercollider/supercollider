//jrh 2002//abstract class that servers as a template for any environment hacksEnvironmentRedirect : Environment {	var <>saveEnvir;			*push { 		^super.new.push;	}		*pop { 		if(this.inside, {			currentEnvironment = currentEnvironment.saveEnvir;		})	}		*inside {		^currentEnvironment.isKindOf(this);	}		//override in subclasses	at { arg key;		^super.at(key)	}		put { arg key, obj;		super.put(key, obj)	}
	
	prAt { arg key;
		^super.at(key)
	}
	
	prPut { arg key, obj;
		super.put(key, obj)
	}		push { 
		//check again		if(this.class.inside, {				this.saveEnvir = currentEnvironment.saveEnvir;			}, { 				this.saveEnvir = currentEnvironment;		});		currentEnvironment = this;
		topEnvironment = this; //to avoid error loss	}		pop {		this.class.pop	}		//PRIVATE//		replaceCurrentEnvir {				this.saveEnvir = currentEnvironment;				currentEnvironment = this;	}	}