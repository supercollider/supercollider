+ Server {

	scope { arg numChannels, index, bufsize = 4096, zoom, rate;
			
			if(scopeWindow.isNil) {
				numChannels = numChannels ? this.options.numOutputBusChannels;
				scopeWindow = 
					Stethoscope.new(this, numChannels, index, bufsize, zoom, rate)
			} {
				if(index.notNil) { scopeWindow.index = index };
				if(rate.notNil) { scopeWindow.rate = rate };
				if(numChannels.notNil) {ÊscopeWindow.numChannels = numChannels };
				if(scopeWindow.bufsize != bufsize) { scopeWindow.allocBuffer(bufsize) };
				if(zoom.notNil) { scopeWindow.zoom = zoom };
				scopeWindow.run;
				
				scopeWindow.window.front;
				
			};
			^scopeWindow
	}
	
}

+ Bus {
	scope { arg bufsize = 4096, zoom;
		^server.scope(numChannels, index, bufsize, zoom, rate)
	}
}


+ Function {
	scope { arg numChannels, outbus = 0, fadeTime = 0.05, bufsize = 4096, zoom;
		var synth;
		synth = this.play(Server.internal, outbus, fadeTime, \addToHead);
		synth.notNil.if { Server.internal.scope(numChannels, outbus, bufsize, zoom, \audio) };
		^synth
	}
}