

CXPlayerControl : AbstractPlayControl {
	var <>bus, proxy, player;
	//here the lazy bus init happens and allocation of client side ressources
	//there is no bundle passing in build

	build { arg argProxy;
		var ok;
		proxy = argProxy;

		if(proxy.isNeutral) {
			source.prepareForPlay; // first initialization for lazy rate detection
			ok = proxy.initBus(source.rate ? 'audio', source.numChannels ? 2);
			source.free;
		};

		bus = proxy.asBus.as(SharedBus);
		player = source.wrapInFader(bus);

		paused = proxy.paused;
		^true // maybe add a test later
	}

	playToBundle { arg bundle, extraArgs, target, addAction=\addToTail;
		if(player.readyForPlay.not) {
			player.prepareToBundle(proxy.group, bundle, false, bus)
		};
		player.makePatchOut(proxy.group, false, bus, bundle);
		player.spawnToBundle(bundle);
		^nil
	}

	stopToBundle { arg bundle, fadeTime=0.02;
		player.releaseAndFreeToBundle(fadeTime, bundle);
	}

	pause { player.stop }
	resume { player.play }
	// moveToGroupToBundle {}

	isPlaying { ^false } // ^source.isPlaying }

}

CXSynthPlayerControl : CXPlayerControl {

	/*
	moveToGroupToBundle { arg bundle, group;
		if(group.isPlaying) {
			bundle.add(["/g_tail", group.nodeID, source.children[0].synth.nodeID]);
		}
	}
	*/

}
