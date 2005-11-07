

// abstract class that dispatches assignment / reference in environments.

EnvironmentRedirect {

	var <envir;
	var <dispatch;
	
	*new { arg envir;
		^super.newCopyArgs(envir ?? { Environment.new(32, Environment.new) })
	}
	*make { arg function;
		^this.new.make(function)
	}
	*use { arg function;
		^this.new.use(function)
	}
	
	// override in subclasses
	
	at { arg key;
		^envir.at(key)
	}
	
	put { arg key, obj;
		envir.put(key, obj);
		dispatch.value(key, obj);
	}
	
	localPut { arg key, obj;
     	envir.put(key, obj)
     }
	
	removeAt { arg key;
		^envir.removeAt(key)
	}
	
	// behave like environment
	
	
	*push {
		^this.new.push;
	}
	
	*pop {
		^Environment.pop;
	}
	
	pop {
		^Environment.pop
	}
	
	push { 
		if(currentEnvironment !== this) {
			Environment.push(this)
		} { "this environment is already current".warn } 
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
	putAll { arg ... dictionaries; 
		dictionaries.do {|dict| 
			dict.keysValuesDo { arg key, value; 
				this.put(key, value) 
			}
		}
	}
	
	choose {
        ^envir.choose
     }
     
     clear { envir.clear }
     
     know_ { arg flag; envir.know = flag }
     know { ^envir.know }
    	
    	doesNotUnderstand { arg selector ... args;
		var func;
		if (this.know) {
			if (selector.isSetter) {
				selector = selector.asGetter;
				^this[selector] = args[0];
			};
			^this.doFunctionPerform(selector, args)

		};
		^this.superPerformList(\doesNotUnderstand, selector, args);
	}
	
	doFunctionPerform { arg selector, args;
		envir[\forward] !? {
			if(envir[selector].isNil) { 
				^envir[\forward].functionPerformList(\value, this, selector, args);
			}
		};
		^this[selector].functionPerformList(\value, this, args);
	}
          
     //// this should maybe go in Environment
     
     makeDocument { arg title, string="";
     	^EnvirDocument.new(this, title, string);
	}
     
     linkDoc { arg doc, pushNow=true;
     	doc = doc ? Document.current;
     	if(doc.isKindOf(EnvirDocument), {
     		doc.envir_(this)
     	}, {
     		if(pushNow) { this.push };
     		"added actions to current doc".inform;
     		doc	.toFrontAction_({ this.push })
     			.endFrontAction_({ this.pop });
     	});
     }
     
     unlinkDoc { arg doc;
     	if(doc.isKindOf(EnvirDocument)) { doc.envir_(nil) ^this };
     	"removed actions from current doc".inform;
     	doc.toFrontAction_(nil).endFrontAction_(nil)
     }
     
     // networking
     dispatch_ { arg disp;
     	     dispatch = disp.envir_(this);
	}
	envir_ { arg argEnvir;
		envir = argEnvir;
		if(dispatch.notNil) { this.dispatch = dispatch };
	}
	
}




LazyEnvir : EnvironmentRedirect {
	
	makeProxy {
		^FuncProxy.new
	}
	
	at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil) {
			proxy = this.makeProxy(key);
			envir.put(key, proxy);
		};
		^proxy
	
	}
	
	put { arg key, obj;
		this.at(key).source_(obj);
		dispatch.value(key, obj); // forward to dispatch for networking
	}
	
	removeAt { arg key;
		var proxy;
		proxy = envir.removeAt(key);
		if(proxy.notNil) { proxy.clear };
	}
	
	localPut { arg key, obj;
     	this.at(key).source_(obj);
     }
     
     
}
