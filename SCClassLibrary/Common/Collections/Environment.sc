Environment : IdentityDictionary {
	classvar <>stack;

	*make { |function|
		^this.new.make(function)
	}
	*use { |function|
		^this.new.use(function)
	}

	make { |function|
		// pushes the Envir, executes function, returns the Envir
		// usually used to create an environment by adding new variables to it.
		var result, saveEnvir;

		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		protect {
			function.value(this)
		} {
			currentEnvironment = saveEnvir
		}
	}
	use { |function|
		// temporarily replaces the currentEnvironment with this,
		// executes function, returns the result of the function
		var result, saveEnvir;

		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		protect {
			result = function.value(this)
		} {
			currentEnvironment = saveEnvir
		};
		^result
	}

	eventAt { |key| ^this.at(key) }
	composeEvents { |event| ^this.copy.putAll(event) }

	*pop {
		if(stack.notNil and: { stack.notEmpty }) { currentEnvironment = stack.pop }
	}
	*push { |envir|
		stack = stack.add(currentEnvironment);
		currentEnvironment = envir
	}
	pop { ^this.class.pop }
	push { this.class.push(this) }

	linkDoc { |doc|
		doc = doc ? Document.current;
		doc.envir_(this)
	}

	unlinkDoc { |doc|
		doc = doc ? Document.current;
		if(doc.envir === this) { doc.envir_(nil) }
	}

}
