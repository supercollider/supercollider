// abstract class that dispatches assignment / reference in environments.

EnvironmentRedirect {

	var <envir;
	var <dispatch;

	*new { |envir|
		^super.newCopyArgs(envir ?? { Environment.new(32, Environment.new) })
	}
	*make { |function|
		^this.new.make(function)
	}
	*use { |function|
		^this.new.use(function)
	}
	*newFrom { |aCollection|
		var newCollection = this.new;
		aCollection.keysValuesDo({ |k, v, i| newCollection.put(k, v) });
		^newCollection
	}

	// override in subclasses

	at { |key|
		^envir.at(key)
	}

	put { |key, obj|
		var allNil = obj.isNil and: { envir.at(key).isNil };
		if(allNil.not) {
			envir.put(key, obj);
			dispatch.value(key, obj)
		}
	}

	removeAt { |key|
		var result = envir.removeAt(key);
		if(result.notNil) { dispatch.value(key, nil) };
		^result
	}

	// if used in a distributed system, these local methods act without
	// calling the dispatch and thereby redistributing the data recursively

	localPut { |key, obj|
		envir.put(key, obj)
	}

	localRemoveAt { |key|
		^envir.removeAt(key)
	}

	// behave like environment


	*push {
		^this.new.push
	}

	*pop {
		^Environment.pop
	}

	pop {
		^Environment.pop
	}

	push {
		if(currentEnvironment !== this) {
			Environment.push(this)
		} { "this environment is already current".warn }
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

	do { |function|
		envir.do(function)
	}

	keysValuesDo { |function|
		envir.keysValuesDo(function)
	}

	keysValuesArrayDo { |argArray, function|
		envir.keysValuesArrayDo(argArray, function)
	}

	findKeyForValue { |val|
		^envir.findKeyForValue(val)
	}

	sortedKeysValuesDo { |function|
		envir.sortedKeysValuesDo(function)
	}

	putAll { |... dictionaries|
		dictionaries.do { |dict|
			dict.keysValuesDo { |key, value|
				this.put(key, value)
			}
		}
	}

	add { |anAssociation|
		this.put(anAssociation.key, anAssociation.value)
	}

	choose {
		^envir.choose
	}

	clear { envir.clear }

	keys { |species = (Set)|
		^envir.keys(species)
	}

	values {
		^envir.values
	}

	know_ { |flag| envir.know = flag }
	know { ^envir.know }

	doesNotUnderstand { |selector ... args|
		var func;
		if(this.know) {
			if(selector.isSetter) {
				selector = selector.asGetter;
				^this[selector] = args[0]
			};
			^this.doFunctionPerform(selector, args)

		};
		^this.superPerformList(\doesNotUnderstand, selector, args)
	}

	doFunctionPerform { |selector, args|
		envir[\forward] !? {
			if(envir[selector].isNil) {
				^envir[\forward].functionPerformList(\value, this, selector, args)
			}
		};
		^this[selector].functionPerformList(\value, this, args)
	}

	printOn { |stream|
		if(stream.atLimit) { ^this };
		stream << this.class.name << "[";
		envir.printItemsOn(stream);
		stream << "]"
	}

	linkDoc { |doc|
		doc = doc ? Document.current;
		doc.envir_(this)
	}

	unlinkDoc { |doc|
		doc = doc ? Document.current;
		if(doc.envir === this) { doc.envir_(nil) }
	}

	// networking
	dispatch_ { |disp|
		dispatch = disp;
		if(disp.respondsTo(\envir_)) { disp.envir_(this) }
	}
	envir_ { |argEnvir|
		envir = argEnvir;
		if(dispatch.notNil) { this.dispatch = dispatch }
	}

}




LazyEnvir : EnvironmentRedirect {
	var <>proxyClass = \Maybe;

	makeProxy {
		^proxyClass.asClass.new
	}

	at { |key|
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil) {
			proxy = this.makeProxy(key);
			envir.put(key, proxy)
		};
		^proxy

	}

	put { |key, obj|
		var proxy = this.at(key);
		var allNil = obj.isNil and: { proxy.source.isNil };
		if(allNil.not) {
			this.at(key).source_(obj);
			dispatch.value(key, obj)
		}
	}

	removeAt { |key|
		var proxy;
		proxy = envir.removeAt(key);
		if(proxy.notNil and: { proxy.source.notNil }) {
			proxy.clear;
			dispatch.value(key, nil)
		};
		^proxy
	}

	// if used in a distributed system, these local methods act without
	// calling the dispatch and thereby redistributing the data recursively

	localRemoveAt { |key|
		var proxy;
		proxy = envir.removeAt(key);
		if(proxy.notNil) { proxy.clear };
		^proxy
	}

	localPut { |key, obj|
		this.at(key).source_(obj)
	}

	copy {
		var result = this.class.new;
		dispatch !? { result.dispatch = dispatch };
		envir.keysValuesDo { |key, val|
			result.envir[key] = val.copy
		};
		^result
	}

	storeOn { |stream|
		if(stream.atLimit) { ^this };
		stream << this.class.name << ".newFrom([";
		stream <<<* envir.getPairs.collect { |x, i| if(i.even) { x } { x.source } };
		stream << "])"
	}

}
