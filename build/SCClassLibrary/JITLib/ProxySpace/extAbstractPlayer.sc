+AbstractPlayer {
	stopSynthToBundle { arg bundle;
		this.children.do({ arg child;
			child.stopSynthToBundle(bundle);
		});
		if(synth.isPlaying ? false,{
			bundle.add(synth.freeMsg);
			synth = nil;
		});
	}

}


+ Object {
	stopSynthToBundle {}
}