
//a dominant neutral element that returns itself as response to any call
//this can be subclassed to respond to specific messages and it can be
//used to carry information across forbidden borders 

N  {
	*new { ^super.new }
	
	//class
	*doesNotUnderstand { arg selector, args;
		this.value; //avoid crash
		^this
	}
	
	*value {}
	
	*performBinaryOpOnSomething {
		^this
	}
	
	//instance
	doesNotUnderstand { arg selector, args;
		this.value; //avoid crash
		^this
	}
	performBinaryOpOnSomething {
		^this
	}
	value {}
	
}
