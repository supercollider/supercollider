
//a dominant neutral element that returns itself as response to any call
//this can be subclassed to respond to specific messages and it can be
//used to carry information across forbidden borders 

N  {
	*new { ^super.new }
	
	//class
	*doesNotUnderstand { arg selector, args;
		this.emptyCall; //avoid crash
		^this
	}
	
	*emptyCall {}
	
	*performBinaryOpOnSomething {
		^this
	}
	
	//instance
	doesNotUnderstand { arg selector, args;
		this.emptyCall; //avoid crash
		^this
	}
	performBinaryOpOnSomething {
		^this
	}
	emptyCall {}
	
}

Debug : N {
	var <>end;
	doesNotUnderstand { arg selector, args;
		if(end.notNil, {
			end = end - 1;
			if(end < 0, { this.halt })
		});
		postln("debug:" + selector + "args:" + args);
		super.doesNotUnderstand(selector, args)
		
	}
	
}

NRef : N {
	var <>value; //value not possibles
	
}