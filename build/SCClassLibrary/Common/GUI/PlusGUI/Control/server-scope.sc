+ Server {

	scope {arg numChannels, startingChannel = 0, bufsize = 4096;
		var numChan, scope, synth;
		if(((this === Server.internal) and: scopeWindow.isNil ), {
			numChan = numChannels ? this.options.numOutputBusChannels;
			scopeBuffer = Buffer.alloc(this, bufsize, numChan, { 
				arg buffer;
				scopeWindow = SCWindow("Internal Server", Rect(300, 10, 660, 280));
				scope = SCScope(scopeWindow, Rect(10,10,640,256));
				scope.bufnum = buffer.bufnum;
				scope.background = Color.black;
				scope.resize = 5;
				synth = SynthDef("server-scope",{ 
					ScopeOut.ar(InFeedback.ar(startingChannel, numChan), buffer.bufnum);  
				}).play(this, addAction: \addToTail);
				// free synth and buffer when window is closed
				scopeWindow.onClose = { 
					synth.free; 
					buffer.free; 
					scopeBuffer = nil; 
					scopeWindow = nil;
				};
				scopeWindow.front;
				// cmdPeriod support
				CmdPeriod.objects.includes(this).not.if({CmdPeriod.add(this);});
			})
		
		}, {scopeWindow.isNil.if({"Server-scope only works with the Internal Server".warn})});
	}
}

// Function support

+ Function {
	scope { arg numChannels, outbus = 0, fadeTime;
		var synth;
		synth = this.play(Server.internal, outbus, fadeTime);
		synth.postln;
		synth.notNil.if({Server.internal.scope(numChannels, outbus); ^synth;});
	}
}