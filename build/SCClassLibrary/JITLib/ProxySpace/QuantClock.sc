
QuantClock : Clock  {
	var <>delta, <>late, <>clock;
	
	*new { arg delta=1.0, late=0.0, clock; 
		^super.newCopyArgs(delta, late, clock ? SystemClock)
	}
	
	sched { arg argDelta, func, divisions=1;
		var dt, quant;
		quant = delta / divisions;
		dt = (quant/2 + this.time + argDelta).round(quant);
		clock.schedAbs(dt, func);
	}
	time {
		^thisThread.time
		// Main.elapsedTime;
	}		
	

}