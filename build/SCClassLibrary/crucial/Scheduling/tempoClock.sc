
+ TempoClock {

	dsched { arg delta,item;
		this.schedAbs(this.elapsedBeats + delta,item)
	}
	//guiClass { ^TempoClockGui }
}

