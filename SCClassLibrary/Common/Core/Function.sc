Function : AbstractFunction {
	var <def, context;
	// a Function is what you get when you write a FunctionDef in your code.
	// it consists of the function's code and the variables in its defining context

	*new { ^this.shouldNotImplement(thisMethod) }

	isFunction { ^true }
	isClosed { ^def.sourceCode.notNil }


	archiveAsCompileString { ^true }
	archiveAsObject { ^true }
	checkCanArchive { if (def.sourceCode.isNil) { "cannot archive open Functions".warn } }
	storeOn { arg stream;
		var args;
		stream << (def.sourceCode ?? {
			args = def.argumentString;
			"{ %\"open Function\" }".format(if(args.notNil) { "| % | ".format(args) } { "" })
		})
	}

	shallowCopy { ^this }

	choose { ^this.value }

	update { |obj, what ... args| this.value(obj, what, *args) }


	// evaluation
	value { arg ... args;
		_FunctionValue
		// evaluate a function with args
		^this.primitiveFailed
	}
	valueArray { arg ... args;
		_FunctionValueArray
		// evaluate a function, if the last argument is an array it will be
		// expanded into separate args.
		^this.primitiveFailed
	}

	valueEnvir { arg ... args;
		_FunctionValueEnvir
		// evaluate a function with args.
		// unsupplied argument names are looked up in the currentEnvironment
		^this.primitiveFailed
	}
	valueArrayEnvir { arg ... args;
		_FunctionValueArrayEnvir
		// evaluate a function, if the last argument is an array it will be
		// expanded into separate args.
		// unsupplied argument names are looked up in the currentEnvironment
		^this.primitiveFailed
	}
	functionPerformList { arg selector, arglist;
		_ObjectPerformList;
		^this.primitiveFailed
	}

	valueWithEnvir { arg envir;
		var prototypeFrame;
		if(envir.isNil) { ^this.value };
		prototypeFrame = def.prototypeFrame.copy;

		def.argNames.do { |name,i|
			var val = envir[name];
			val !? { prototypeFrame[i] = val };
		};
		//	postf("argNames: % prototypeFrame: %\n", def.argNames, prototypeFrame);

		// evaluate a function, using arguments from the supplied environment
		// slightly faster than valueEnvir and does not replace the currentEnvironment
		^this.valueArray(prototypeFrame)
	}

	performWithEnvir { |selector, envir|
		if(selector === \value) { ^this.valueWithEnvir(envir) };
		^super.performWithEnvir(selector, envir)
	}

	performKeyValuePairs { |selector, pairs|
		var envir;
		if(selector !== \value) {
			^this.superPerform(\performKeyValuePairs, pairs)
		};

		envir = this.def.makeEnvirFromArgs;
		envir.putPairs(pairs);

		^this.valueWithEnvir(envir)
	}

	numArgs { ^def.numArgs }		// return number of arguments to the function
	numVars { ^def.numVars }		// return number of variables in the function
	varArgs { ^def.varArgs }		// return boolean whether function has ellipsis argument

	loop {
		// loop is supported magically by the compiler,
		// thus it can be implemented in terms of itself
		loop { this.value };
	}

	block {
		^this.value {|val| ^val };
	}
	//	block {
	//		var result;
	//		try {
	//			result = this.value #{|val| Break(val).throw };
	//		}{|error|
	//			if (error.class == Break) {
	//				^error.value
	//			}{
	//				error.throw
	//			}
	//		}
	//		^result
	//	}

	asRoutine {
		^Routine.new(this)
	}

	dup { arg n = 2;
		^Array.fill(n, this)
	}
	sum { arg n = 2;
		var sum = 0;
		n.do {|i| sum = sum + this.value(i) };
		^sum
	}

	defer { arg delta;
		if (delta.isNil and: {this.canCallOS}) {
			this.value
		}{
			AppClock.sched(delta ? 0, { this.value; nil })
		}
	}

	thunk { ^Thunk(this) }

	// Pattern support
	transformEvent { arg event;
		^this.value(event)
	}

	// ControlView support
	set { arg ... args; ^this.valueArray(args) }
	get { arg prevVal; ^prevVal }

	fork { arg clock, quant, stackSize;
		^Routine(this, stackSize).play(clock, quant);
	}

	forkIfNeeded { arg clock, quant, stackSize;
		if(thisThread.isKindOf(Routine), this, { ^this.fork(clock, quant, stackSize) });
		^thisThread;
	}

	whenTrueWithin { |timeout = 3, thenFunc, elseFunc, dt = 0.1, clock = (SystemClock)|
		var stillWaiting = true;

		clock.sched(timeout, {
			if (stillWaiting) {
				stillWaiting = false;
				if (this.value, thenFunc, elseFunc);
			}
		});

		clock.sched(dt, {
			if (stillWaiting and: { this.value }) {
				stillWaiting = false;
				thenFunc.value;
				nil
			} {
				// reschedule
				dt
			}
		})
	}

	awake { arg beats, seconds, clock;
		var time = seconds; // prevent optimization
		^this.value(beats, seconds, clock)
	}

	cmdPeriod { this.value }


	bench { arg print = true;
		var dt;
		var t0 = Main.elapsedTime;
		this.value;
		dt = Main.elapsedTime - t0;
		if (print) { Post << "time to run: " << dt << " seconds.\n"; }
		^dt
	}

	protect { arg handler;
		var result;
		result = this.prTry;
		if (result.isException) {
			handler.value(result);
			result.throw;
		}{
			handler.value; // argument should be nil if there was no exception.
			^result
		};
	}

	try { arg handler;
		var result = this.prTry;
		if (result.isException) { ^handler.value(result); }
		{ ^result }
	}
	prTry {
		var result, thread = thisThread;
		var next = thread.exceptionHandler,
		wasInProtectedFunc = Exception.inProtectedFunction;
		thread.exceptionHandler = {|error|
			thread.exceptionHandler = next; // pop
			^error
		};
		Exception.inProtectedFunction = true;
		result = this.value;
		Exception.inProtectedFunction = wasInProtectedFunc;
		thread.exceptionHandler = next; // pop
		^result
	}

	handleError { arg error; ^this.value(error) }

	case { arg ... cases;
		cases = [this] ++ cases;
		cases.pairsDo { | test, trueFunc |
			if (test.value) { ^trueFunc.value };
		};
		if (cases.size.odd) { ^cases.last.value };
		^nil
	}

	r { ^Routine(this) }
	p { ^Prout(this) }

	matchItem { arg item;
		^this.value(item)
	}

	// scale suppoert

	performDegreeToKey { arg scaleDegree, stepsPerOctave = 12, accidental = 0;
		^this.value(scaleDegree, stepsPerOctave, accidental)
	}

	// multichannel expand function return values

	flop {
		if(def.argNames.isNil) { ^this };
		^{ |... args| args.flop.collect(this.valueArray(_)) }
	}


	envirFlop {
		var func = this.makeFlopFunc;
		^{ |... args|
			func.valueArrayEnvir(args).collect(this.valueArray(_))
		}
	}

	makeFlopFunc {
		if(def.argNames.isNil) { ^this };

		^interpret(
			"#{ arg " ++ " " ++ def.argumentString(true) ++ "; "
			++ "[ " ++ def.argumentString(false) ++ " ].flop };"
		)
	}

	// attach the function to a specific environment
	inEnvir { |envir|
		envir ?? { envir = currentEnvironment };
		^{ |... args| envir.use({ this.valueArray(args) }) }
	}

	asBuffer { |duration = 0.01, target, action, fadeTime = (0)|
		var buffer, def, synth, name, numChannels, rate, server;
		target = target.asTarget;
		server = target.server;


		name = this.hash.asString;
		def = SynthDef(name, { |bufnum|
			var	val = this.value;
			if(val.isValidUGenInput.not) {
				val.dump;
				Error("reading signal failed: % is no valid UGen input".format(val)).throw
			};
			val = UGen.replaceZeroesWithSilence(val.asArray);
			rate = val.rate;
			if(rate == \audio) { // convert mixed rate outputs:
				val = val.collect { |x| if(x.rate != \audio) { K2A.ar(x) } { x } }
			};
			numChannels = val.size.max(1);
			if(fadeTime > 0) {
				val = val * EnvGen.kr(Env.linen(fadeTime, duration - (2 * fadeTime), fadeTime))
			};
			RecordBuf.perform(RecordBuf.methodSelectorForRate(rate), val, bufnum, loop: 0);
			Line.perform(Line.methodSelectorForRate(rate), dur: duration, doneAction: 2);
		});

		buffer = Buffer.new(server);

		Routine.run {
			var numFrames, running;
			running = server.serverRunning;
			if(running.not) { server.bootSync; 1.wait };
			numFrames = duration * server.sampleRate;
			if(rate == \control) { numFrames = numFrames / server.options.blockSize };
			buffer.numFrames = numFrames;
			buffer.numChannels = numChannels;
			buffer = buffer.alloc(numFrames, numChannels);
			server.sync;
			def.send(server);
			server.sync;
			synth = Synth(name, [\bufnum, buffer], target, \addAfter);
			OSCFunc({
				action.value(buffer);
				server.sendMsg("/d_free", name);
			}, '/n_end', server.addr, nil, [synth.nodeID]).oneShot;
		};

		^buffer
	}

	loadToFloatArray { |duration = 0.01, target, action|
		this.asBuffer(duration, target, { |buffer|
			buffer.loadToFloatArray(action: { |array|
				action.value(array, buffer);
				buffer.free
			})
		})
	}

	getToFloatArray { |duration = 0.01, target, action, wait = 0.01, timeout = 3|
		this.asBuffer(duration, target, { |buffer|
			buffer.getToFloatArray(0, wait: wait, action: { |array|
				action.value(array, buffer);
				buffer.free
			})
		})
	}

}

Thunk : AbstractFunction {
	// a thunk is an unevaluated value.
	// it gets evaluated once and then always returns that value.
	// also known as a "promise" in Scheme.
	// thunks have no arguments.
	var function, value;

	*new { arg function;
		^super.newCopyArgs(function)
	}
	value {
		^value ?? { value = function.value; function = nil; value }
	}
	valueArray { ^this.value }
	valueEnvir { ^this.value }
	valueArrayEnvir { ^this.value }
}

UGenThunk : Thunk {
	var ugens;
	value {
		var res;
		ugens = ugens ?? { IdentityDictionary.new };
		res = ugens.at(UGen.buildSynthDef);
		if(res.isNil) { res = super.value; ugens.put(UGen.buildSynthDef, res) };
		^res
	}
}
