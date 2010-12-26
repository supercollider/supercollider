+ Server {

	scope { arg numChannels, index, bufsize = 4096, zoom, rate;
		var gui;
		numChannels = (numChannels ? 2).min(16);
		if(scopeWindow.isNil) {
			gui = GUI.current;
			scopeWindow =
				gui.stethoscope.new(this, numChannels, index, bufsize, zoom, rate, nil,
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

	freqscope {
		GUI.freqScope.new;
	}


}

+ Bus {
	scope { arg bufsize = 4096, zoom;
		^server.scope(numChannels, index, bufsize, zoom, rate);
	}
}


+ Function {
	scope { arg numChannels, outbus = 0, fadeTime = 0.05, bufsize = 4096, zoom;
		var synth, synthDef, bytes, synthMsg, outUGen, server, gui;
		gui = GUI.current;
		server = gui.stethoscope.defaultServer;
		if(server.serverRunning.not) {
			(server.name.asString ++ " server not running!").postln;
			^nil
		};
		synthDef = this.asSynthDef(name: SystemSynthDefs.generateTempName, fadeTime:fadeTime);
		outUGen = synthDef.children.detect { |ugen| ugen.class === Out };

		numChannels = numChannels ?? { if(outUGen.notNil) { (outUGen.inputs.size - 1) } { 1 } };
		synth = Synth.basicNew(synthDef.name, server);
		bytes = synthDef.asBytes;
		synthMsg = synth.newMsg(server, [\i_out, outbus, \out, outbus], \addToHead);
		server.sendMsg("/d_recv", bytes, synthMsg);
		server.scope(numChannels, outbus, bufsize, zoom, outUGen.rate);
		^synth
	}

	freqscope {
		this.play(GUI.stethoscope.defaultServer);
		GUI.freqScope.new;
	}
}

