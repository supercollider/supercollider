+ Server {
	scope { arg numChannels, index = 0, bufsize = 4096, zoom = (1), rate = \audio, position = nil;
		numChannels = numChannels ?? { if (index == 0) { options.numOutputBusChannels } { 2 } };

		if(position.isNil) {
			if(scopeWindow.isNil) {
				scopeWindow = Stethoscope(this, numChannels, index, bufsize, zoom, rate, nil,
					this.options.numBuffers);
				// prevent buffer conflicts by using reserved bufnum
				scopeWindow.window.onClose = scopeWindow.window.onClose.addFunc({ scopeWindow = nil });
			} {
				scopeWindow.setProperties(numChannels, index, bufsize, zoom, rate);
				scopeWindow.run;
				scopeWindow.window.front;
			}
		} {
			var window = Window("Stethoscope", Rect(position.x, position.y, 250, 250));

			if(scopeWindow.isNil) {
				scopeWindow = Stethoscope(this, numChannels, index, bufsize, zoom, rate, window.view,
					this.options.numBuffers);
				// prevent buffer conflicts by using reserved bufnum
				window.onClose = window.onClose.addFunc({ scopeWindow = nil });
				window.front;
			} {
				scopeWindow.setProperties(numChannels, index, bufsize, zoom, rate);
				scopeWindow.run;
			}
		};
		^scopeWindow
	}

	freqscope {
		^FreqScope.new(server: this);
	}
}

+ Bus {
	scope { arg bufsize = 4096, zoom;
		^server.scope(numChannels, index, bufsize, zoom, rate);
	}
}


+ Function {
	scope { arg numChannels, outbus = 0, fadeTime = 0.05, bufsize = 4096, zoom, position = nil;
		var synth, synthDef, bytes, synthMsg, outUGen, server;

		server = Server.default;
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
		server.scope(numChannels, outbus, bufsize, zoom, outUGen.rate, position);
		^synth
	}

	freqscope {
		var server = Server.default;
		this.play(server);
		^FreqScope.new(server: server)
	}
}
