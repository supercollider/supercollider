+ Server {

	scope { arg numChannels, index, bufsize = 4096, zoom, rate;
			
			if(scopeWindow.isNil) {
				numChannels = numChannels ? this.options.numOutputBusChannels;
				scopeWindow = 
					Stethoscope.new(this, numChannels, index, bufsize, zoom, rate, nil, 
						this.options.numBuffers); 
						// prevent buffer conflicts by using reserved bufnum
				CmdPeriod.add(this);
			} {
				scopeWindow.setProperties(numChannels, index, bufsize, zoom, rate);
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
		var synth, synthDef, bytes, synthMsg, outUGen, server;
		server = Server.internal;
		if(server.serverRunning.not) { "internal server not running!".postln; ^nil };
		def = this.asSynthDef(fadeTime:fadeTime);
		outUGen = def.children.detect { |ugen| ugen.class === Out };
		numChannels = numChannels ? (outUGen.inputs.size - 1);
		synth = Synth.basicNew(def.name, server);
		bytes = def.asBytes;
		synthMsg = synth.newMsg(server, [\i_out, outbus, \out, outbus], \addToHead);
		server.sendMsg("/d_recv", bytes, synthMsg);
		server.scope(numChannels, outbus, bufsize, zoom, outUGen.rate);
		^synth
	}
}
