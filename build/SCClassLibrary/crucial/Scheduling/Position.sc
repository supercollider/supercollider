
Position  {
	
	var <current=0,<previous=0;
	
	value { ^current }
	value_ { arg val;
		previous = current;
		current = val;
		this.changed;
	}
	
	next {
		previous = current;
		current = current + 1;
		this.changed;
		^current
	}
	
}