//jrh 2002
//abstract class that servers as a template for any environment hacks

EnvironmentRedirect {
	var <>envir, <name;
	classvar <>saveEnvir, <>saveTopEnvir;
	
	*new { arg name;
		^super.newCopyArgs(Environment.new, name.asSymbol)
	}
	
	*push { arg name;
		^super.new(name).push;
	}
	
	*pop { 
		var curr;
		curr = currentEnvironment;
		if(curr.isKindOf(this), {
			currentEnvironment = saveEnvir;
			topEnvironment = saveTopEnvir;
		}, {
			"outside environment already".inform;
		})
	
	}
	
	push { 
		if(currentEnvironment === this, { ^topEnvironment = this });
		if(currentEnvironment.isKindOf(this.class),
			{ currentEnvironment.pop });
		this.class.saveEnvir = currentEnvironment;
		this.class.saveTopEnvir = topEnvironment;
		currentEnvironment = this;
		topEnvironment = this; //to avoid error loss
	}
	
	pop {
		this.class.pop
	}

	
	
	//override in subclasses
	
	at { arg key;
		^envir.at(key)
	}
	
	put { arg key, obj;
		envir.put(key, obj)
	}
	
	
		
	// behave like my environment
	
	
	use { arg function;
		// temporarily replaces the currentEnvironment with this, 
		// executes function, returns the result of the function
		var result, saveEnvir;
		
		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		result = function.value(this);
		currentEnvironment = saveEnvir;
		^result
	}
	
	do { arg function;
		envir.do(function)
	}

	keysValuesDo { arg function;
		envir.keysValuesDo(function);
	}
	
	keysValuesArrayDo { arg argArray, function;
		envir.keysValuesArrayDo(argArray, function);
	}
	findKeyForValue { arg val;
		^envir.findKeyForValue(val)
	}
	
	choose {
        ^envir.choose
     }
     
     makeDocument { arg title, string, backcolor, stringcolor;
     	var doc;
     	doc = Document.new("", "untitled proxyspace", false)
     		.background_(backcolor ? rgb(223, 223, 223))
			.stringColor_(stringcolor ? rgb(0,0,0));
		this.linkDoc(doc, false);
	 }
     
     linkDoc { arg doc, instantPush=true;
     	doc = doc ? Document.current;
     	if(instantPush) {this.push};
     	doc	.toFrontAction_({ this.push })
     		.endFrontAction_({ this.pop });
     }
}
