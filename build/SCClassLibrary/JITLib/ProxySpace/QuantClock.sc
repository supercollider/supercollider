
QuantClock  {
	var <>delta, <>late, <>clock;
	
	*new { arg delta=1.0, late=0.0, clock; 
		^super.newCopyArgs(delta, late, clock ? SystemClock)
	}
	
	sched { arg func, divisions=1, offset=0; //offset in divisions
		var t, dt, quant;
		quant = delta / divisions;
		offset = late + (quant * offset);
		t = this.time;
		dt = (quant/2 + t).round(quant)  + offset;
		
		clock.schedAbs(dt, func);
	}
	time {
		^thisThread.time
		// Main.elapsedTime;
	}		
	

}