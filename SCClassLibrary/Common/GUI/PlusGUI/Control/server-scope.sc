+ Server {
	scope { arg numChannels, index = 0, bufsize = 4096, zoom = 1, rate = \audio;
		numChannels = (numChannels ? 2).min(16);
		zoom = zoom ? 1.0;

		if(scopeWindow.isNil) {
			if ((GUI.id == \qt) and: (this.isLocal)) {
				scopeWindow = \QStethoscope2.asClass.new(this, numChannels, index, bufsize, 1024 * zoom.asFloat.reciprocal, rate);
			} {
				scopeWindow = Stethoscope(this, numChannels, index, bufsize, zoom, rate, nil,
					this.options.numBuffers);
				// prevent buffer conflicts by using reserved bufnum
			};
			scopeWindow.window.onClose = scopeWindow.window.onClose.addFunc({ scopeWindow = nil });
			ServerTree.add(this, this);
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
		var synth, synthDef, bytes, synthMsg, outUGen, server;

		if (GUI.id == \qt) {
			server = Server.default;
		} {
			server = GUI.stethoscope.defaultServer;
			if(server.serverRunning.not) {
				(server.name.asString ++ " server not running!").postln;
				^nil
			}
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
		var server = if (GUI.id == \qt) { Server.default } { GUI.stethoscope.defaultServer };
		this.play(server);
		^GUI.freqScope.new
	}
}
