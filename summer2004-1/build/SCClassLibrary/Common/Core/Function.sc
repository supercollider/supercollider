Function : AbstractFunction {
	var <def, context;
	// a Function is what you get when you write a FunctionDef in your code.
	// it consists of the function's code and the variables in its defining context

	*new { ^this.shouldNotImplement(thisMethod) }
	
	isClosed { ^def.sourceCode.notNil }

	storeOn { arg stream; stream << def.sourceCode }
	
	shallowCopy { ^this }
	
	choose { ^this.value }
	
	
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
	
	loop {
		// loop is supported magically by the compiler,
		// thus it can be implemented in terms of itself
		loop { this.value };
	}

	block {
		this.value {|val| ^val };
	}
	
	asRoutine {
		^Routine.new(this)
	}
			
	dup { arg n = 2;
		var array;
		n.do {|i| array = array.add(this.value(i)) };
		^array
	}
	sum { arg n = 2;
		var sum = 0;
		n.do {|i| sum = sum + this.value(i) };
		^sum
	}
	
	defer { arg delta = 0;
		AppClock.sched(delta, this)
	}

	// Pattern support
	transformEvent { arg event;
		^this.value(event)
	}

	// ControlView support
	set { arg ... args; ^this.valueArray(args) }
	get { arg prevVal; ^prevVal }
	
	fork { arg clock, quant=0.0, stackSize=512;
		^Routine(this, stackSize).play(clock, quant);
	}
	
	awake { arg beats, seconds, clock;
		var time;
		time = seconds; // prevent optimization
		^this.value(beats, seconds, clock)
	}
	plot {
		^thisMethod.notYetImplemented
	}
	cmdPeriod { this.value }	
	
	<> { arg that;
		// function composition
		^{|...args| this.valueArray(that.valueArray(args)) }
	}

	bench { arg print = true;
		var t0, dt;
		t0 = Main.elapsedTime;
		this.value;
		dt = Main.elapsedTime - t0;
		if (print) { Post << "time to run: " << dt << " seconds.\n"; }
		^dt
	}
	
	protect { arg handler;
		var error;
		error = this.prTry;
		handler.value(error);
		error.throw;
	}
	
	try { arg handler;
		var error;
		error = this.prTry;
		if (error.notNil) { handler.value(error); }
	}
	prTry {
		var next;
		next = thisThread.exceptionHandler;
		thisThread.exceptionHandler = {|error| 
			thisThread.exceptionHandler = next; // pop
			^error 
		};
		this.value;
		thisThread.exceptionHandler = next; // pop
		^nil
	}
	
	handleError { arg error; ^this.value(error) }
}


