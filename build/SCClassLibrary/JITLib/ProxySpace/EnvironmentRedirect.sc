
//abstract class that servers as a template for any environment hacks

EnvironmentRedirect {
	var <>envir, <name;
	classvar <>saveEnvir, <>saveTopEnvir;
	
	*new { arg name;
		^super.newCopyArgs(Environment.new, name.asSymbol)
	}
	
	*push { arg name;
		^this.new(name).push;
	}
	
	*pop { 
		^Environment.pop;
	}
	
	pop {
		^Environment.pop
	}
	
	push { 
		// avoid chaining, maybe move down
		if(currentEnvironment !== this, {
			topEnvironment = this;
			Environment.push(this);
		});
		
	}
	
	//override in subclasses
	
	at { arg key;
		^envir.at(key)
	}
	
	put { arg key, obj;
		envir.put(key, obj)
	}
		
		
	// behave like my environment
	
	removeAt { arg key;
		^envir.removeAt(key)
	}

	
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
	sortedKeysValuesDo { arg function;
		envir.sortedKeysValuesDo(function);
	}
	
	choose {
        ^envir.choose
     }
     
     makeDocument { arg title, string="";
     	^EnvirDocument.new(this, title, string);
	}
     
     linkDoc { arg doc, instantPush=true;
     	doc = doc ? Document.current;
     	if(doc.isKindOf(EnvirDocument), {
     		doc.envir_(this)
     	}, {
     		if(instantPush) {this.push};
     		"added actions to current doc".inform;
     		doc	.toFrontAction_({ this.push })
     			.endFrontAction_({ this.pop });
     	});
     }
}
