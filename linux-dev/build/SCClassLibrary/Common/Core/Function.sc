Function : AbstractFunction {
	var <def, context;
	// a Function is what you get when you write a FunctionDef in your code.
	// it consists of the function's code and the variables in its defining context

	*new { ^this.shouldNotImplement(thisMethod) }
	*newClear { ^this.shouldNotImplement(thisMethod) }
	
	isClosed { _FunctionIsClosed }
	
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
	
	loop {
		// loop is supported magically by the compiler,
		// thus it can be implemented in terms of itself
		loop({ this.value });
	}
	
	block {
		this.value({ arg val; ^val });
	}
	
	asRoutine {
		^Routine.new(this)
	}
			
	draw { this.value }

	dup { arg n = 2;
		var array;
		n.do({ arg i; array = array.add(this.value(i)) });
		^array
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
	
	
	awake { arg inTime;
		var time;
		time = inTime; // prevent optimization
		^this.value(inTime)
	}
//	play { arg clock;
//		clock = clock ? SystemClock;
//		clock.play(this);
//	}
}


