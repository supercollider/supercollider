
//abstract class that servers as a template for any environment hacks

EnvironmentRedirect {
	var <>envir, <name;
	var <>crashProof=true;
	
	*new { arg name, envir;
		^super.newCopyArgs(nil, name).init(envir)
	}
	init { arg argEnvir; if(argEnvir.notNil) {Êenvir = argEnvir }Ê{ this.clear } }
	
	clear { arg size=32; envir = Environment.new(size, Environment.new)  }
	
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
		// avoid chaining
		if(currentEnvironment !== this) {
			if(crashProof) { topEnvironment = this }; // Object-halt restores to top envir
			Environment.push(this);
		};
		
	}
	
	// override in subclasses
	// behave like my environment
	
	at { arg key;
		^envir.at(key)
	}
	
	put { arg key, obj;
		envir.put(key, obj)
	}
	
	removeAt { arg key;
		^envir.removeAt(key)
	}
	
	make { arg function;
		// pushes the Envir, executes function, returns the Envir
		// usually used to create an environment by adding new variables to it.
		var result, saveEnvir;
		
		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		protect {
			function.value(this);
		}{
			currentEnvironment = saveEnvir;
		};
	}
	
	use { arg function;
		// temporarily replaces the currentEnvironment with this, 
		// executes function, returns the result of the function
		var result, saveEnvir;
		
		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		protect {
			result = function.value(this);
		}{
			currentEnvironment = saveEnvir;
		};
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
     
     //// this should maybe go in Environment
     
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




