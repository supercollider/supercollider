

///////////////// cx players ////////////

+SynthlessPlayer {

	proxyControlClass {
		^CXPlayerControl
	}

	wrapInFader { ^this }

}

+AbstractPlayer {

	proxyControlClass {
			^CXSynthPlayerControl
	}

	wrapInFader { arg bus;
			var n;
			n = bus.numChannels ? this.numChannels;

			^EnvelopedPlayer(
				this,
				Env.asr(1, 1, 1),
				n
			);

	}

	releaseAndFreeToBundle { arg releaseTime=(0.0), bundle;
		this.releaseToBundle(releaseTime, bundle);
		if(releaseTime > 0.01, {
			bundle.addFunction({
				SystemClock.sched(releaseTime + 0.1, {
					this.free;
					nil;
				})
			});
		});
	}

}

+Instr {
	proxyControlClass {
			^CXSynthPlayerControl
	}

	wrapInFader { arg bus;
		^Patch(this.name).wrapInFader(bus)
	}
}


