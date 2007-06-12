

///////////////// cx players ////////////

+SynthlessPlayer {
	
	proxyControlClass {
		^CXPlayerControl
	}
	
	wrapInFader {}
	
	
}

+AbstractPlayer {
	
	proxyControlClass {
			^CXSynthPlayerControl
	}
	
	makeProxyControl { arg channelOffset=0, proxy;
			var player;
			//this.prepareToPlayWithProxy(proxy); //do it here for now.
			player = if(proxy.isNeutral) { this } { this.wrapInFader(proxy) }Õ;
			^this.proxyControlClass.new(player, channelOffset); 
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
	
}


+Instr {
	
	makeProxyControl { arg channelOffset=0, proxy;
		^Patch(this.name).makeProxyControl(channelOffset, proxy)
	}
	
}

