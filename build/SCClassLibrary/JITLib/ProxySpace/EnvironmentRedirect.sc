
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
		Environment.pop;
	}
	
	pop {
		Environment.pop
	}
	
	push { 
		topEnvironment = this;
		if(currentEnvironment !== this, {  
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
