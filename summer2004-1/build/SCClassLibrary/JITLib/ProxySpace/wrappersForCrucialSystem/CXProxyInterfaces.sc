

CXPlayerControl : AbstractPlayControl {
	var <>bus;
	//here the lazy bus init happens and allocation of client side ressources
	//there is no bundle passing in build
	
	build { arg proxy;
		var ok;
		if(proxy.isNeutral) {
			source.prepareForPlay; // first initialization for lazy rate detection
			ok = proxy.initBus(source.rate ? 'audio', source.numChannels ? 2);
			source.free;
			
		};
		bus = proxy.asBus.as(SharedBus);
		source.prepareForPlay(nil, true, bus);
		ok = source.readyForPlay;
		paused = proxy.paused;
		^ok
	}
	
	playToBundle { arg bundle, extraArgs, target, addAction=\addToTail;
		//var group;
		var f;
		if(paused.not) {
			// we'll need channel offset maybe.
			//group = Group.newToBundle(bundle, target, addAction);
			source.prepareToBundle(nil, bundle);
			f = {ÊCmdPeriod.remove(f); source.free; };
			CmdPeriod.add(f); 
			if(source.readyForPlay.not) {
				source.makePatchOut(nil, true, bus, bundle);
				"made new patch out for player".debug;
			};
			source.spawnOnToBundle(nil, bus, bundle); //proxy.group
			// this.moveToGroupToBundle(bundle, proxy.group);
		};
		^nil
		
		
	}
	 
	stopToBundle { arg bundle, fadeTime=0.02;
		source.releaseToBundle(fadeTime, bundle);
	}
	
	freeToBundle { arg bundle, fadeTime;
//		bundle.addSchedFunction({ source.freeHeavyResources }, fadeTime);
//		source.freeToBundle(bundle);
		bundle.addSchedFunction({ source.free }, fadeTime);

	}
	
	
	pause { source.stop }
	resume { source.play }
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
