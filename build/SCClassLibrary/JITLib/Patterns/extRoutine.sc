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
