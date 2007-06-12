
QuantClock : Clock  {
	var <>period, <>late, <>clock;
	
	*new { arg period=1.0, late=0.0, clock; 
		^super.newCopyArgs(period, late, clock ? SystemClock)
	}
	
	sched { arg delta, func, divisions=1;
		var dt, quant, hq;
		quant = period / divisions;
		hq = quant*0.5;
		if(delta >= hq, { hq = 0 });
		dt = (hq + delta + this.time).round(quant);
		clock.schedAbs(dt, func);
	}
	time {
		^thisThread.time
		// Main.elapsedTime;
	}		
	

}