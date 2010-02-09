
/*
	much more useful than it looks.
	a sequencer can use this, its gui can be a dependant of the Position
	and thus get update messages whenever the position changes.
	the gui can check the previous step in order to reset the last highlighted
	position on the gui
*/
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
	wrapNext { arg min,max;
		previous = current;
		current = (current + 1).wrap(min,max);
		this.changed;
		^current
	}
}