+SynthDef {
	*prNew { arg name;
		^super.new.name_(name)
	}
}

+Node {
	prIsPlaying { arg flag; isPlaying = true }

}