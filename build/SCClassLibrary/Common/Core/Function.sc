Function : AbstractFunction {
	var <def, context;
	// a Function is what you get when you write a FunctionDef in your code.
	// it consists of the function's code and the variables in its defining context

	*new { ^this.shouldNotImplement(thisMethod) }
	
	isClosed { ^def.sourceCode.notNil }

	storeOn { arg stream; stream << (def.sourceCode ? "{ \"open Function\" }"); }
	archiveAsCompileString { ^true }
	archiveAsObject { ^true }
	checkCanArchive { if (def.sourceCode.isNil) { "cannot archive open Functions".warn } }
	
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
		var array;
		array = Array(n);
		n.do {|i| array.add(this.value(i)) };
		^array
	}
	sum { arg n = 2;
		var sum = 0;
		n.do {|i| sum = sum + this.value(i) };
		^sum
	}
	
	defer { arg delta = 0;
		if (delta == 0 and: {this.canCallOS}) {
			this.value
		}{
			AppClock.sched(delta, { this.value; nil })
		}
	}

	// Pattern support
	transformEvent { arg event;
		^this.value(event)
	}

	// ControlView support
	set { arg ... args; ^this.valueArray(args) }
	get { arg prevVal; ^prevVal }
	
	fork { arg clock, quant=0.0, stackSize=64;
		^Routine(this, stackSize).play(clock, quant);
	}
	
	awake { arg beats, seconds, clock;
		var time;
		time = seconds; // prevent optimization
		^this.value(beats, seconds, clock)
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
		var result;
		result = this.prTry;
		if (result.isException) { ^handler.value(result); } { ^result }
	}
	prTry {
		var next, result;
		next = thisThread.exceptionHandler;
		thisThread.exceptionHandler = {|error| 
			thisThread.exceptionHandler = next; // pop
			^error 
		};
		result = this.value;
		thisThread.exceptionHandler = next; // pop
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
}


