+ Server {

	scope { arg numChannels, startingChannel = 0, bufsize = 4096, zoom;
			
			if(scopeWindow.isNil) {
				numChannels = numChannels ? this.options.numOutputBusChannels;
				scopeWindow = Stethoscope.new(this, bufsize, numChannels, zoom)
			} {
				scopeWindow.index = startingChannel;
				if(numChannels.notNil) {ÊscopeWindow.numChannels = numChannels };
				if(scopeWindow.numFrames != bufsize) { scopeWindow.allocBuffer(bufsize) };
				if(zoom.notNil) { scopeWindow.zoom = zoom };
				scopeWindow.run;
				
				scopeWindow.window.front;
				
			};
			^scopeWindow
	}
	
}

+ Bus {
	scope { arg bufsize = 4096, zoom;
		^server.scope(numChannels, index, bufsize, zoom)
	}
}


+ Function {
	scope { arg numChannels, outbus = 0, fadeTime=0.05, bufsize = 4096, zoom;
		var synth;
		synth = this.play(Server.internal, outbus, fadeTime, \addToHead);
		synth.notNil.if { Server.internal.scope(numChannels, outbus, bufsize, zoom) };
		^synth
	}
}