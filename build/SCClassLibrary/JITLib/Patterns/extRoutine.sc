+Routine {
	embed { arg inval;
		func.value(inval)
	}

}

+Task {
	embed { arg inval;
		originalStream.embed(inval)
	}
	refresh {
		stream = originalStream
	}
	isPlaying {
		^stream != nil
	}
}

+Pattern {
	loop {
		^Pn(this, inf)
	}

}

+Stream {
	loop {
		^Routine.new({ arg inevent;
			inf.do({
				inevent = this.embedInStream(inevent);
			});
		});
	}
}

+Object {
	loop {}
}


+UGen {

	lag { arg lagTime=0.1;
		^Lag.multiNew(this.rate, this, lagTime)
	}
	lag2 { arg lagTime=0.1;
		^Lag2.multiNew(this.rate, this, lagTime)
	}
	lag3 { arg lagTime=0.1;
		^Lag3.multiNew(this.rate, this, lagTime)
	}

}


