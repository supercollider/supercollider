Environment : IdentityDictionary {
	classvar <>stack;

	*make { arg function;
		^this.new.make(function)
	}
	*use { arg function;
		^this.new.use(function)
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

	eventAt { arg key; ^this.at(key) }
	composeEvents { arg event; ^this.copy.putAll(event) }

	*pop {
		if(stack.notNil and: { stack.notEmpty }) { currentEnvironment = stack.pop };
	}
	*push { arg envir;
		stack = stack.add(currentEnvironment);
		currentEnvironment = envir;
	}
	pop { ^this.class.pop }
	push { this.class.push(this) }

	linkDoc { arg doc, pushNow = true;
		if(Platform.ideName == "scqt") { Error("Current platform doesn't support linking environments to documents.").throw };
     	doc = doc ? Document.current;
     	doc.envir_(this);
     	if(pushNow and: { currentEnvironment !== this }) { this.push };
     }

     unlinkDoc { arg doc, popNow = false;
		if(Platform.ideName == "scqt") { Error("Current platform doesn't support linking environments to documents.").throw };
     	doc = doc ? Document.current;
     	if(doc.envir === this) { doc.envir_(nil) };
     	if(popNow and:  { currentEnvironment === this }) { this.pop };
     }

}
