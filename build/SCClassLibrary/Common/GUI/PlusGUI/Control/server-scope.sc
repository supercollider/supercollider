+ Server {

	scope {arg numChannels, startingChannel = 0, bufsize = 4096, zoom = 1;
		var numChan, scope, synth;
		if((this === Server.internal) and: scopeWindow.isNil ) {
			numChan = numChannels ? this.options.numOutputBusChannels;
			scopeBuffer = Buffer.alloc(this, bufsize, numChan);
			scopeWindow = SCWindow("Internal Server", Rect(300, 10, 660, 280));
			scope = SCScope(scopeWindow, Rect(10,10,640,256));
			scope.bufnum = scopeBuffer.bufnum;
			scope.background = Color.black;
			scope.resize = 5;
			scope.xZoom = zoom;
			synth = SynthDef("server-scope",{ 
					ScopeOut.ar(InFeedback.ar(startingChannel, numChan), scopeBuffer.bufnum);  
			}).play(this, addAction: \addToTail);
				
			// free synth and buffer when window is closed
			scopeWindow.onClose = { 
			synth.free; 
			scopeBuffer.free; 
			scopeWindow = scopeBuffer = nil; 
			};
			scopeWindow.front;
			CmdPeriod.add(this);
		} { 
			if(scopeWindow.isNil) {"Server-scope only works with the Internal Server".warn }
		}
	}
	
}

// Function support

+ Function {
	scope { arg numChannels, outbus = 0, fadeTime=0.05;
		var synth;
		synth = this.play(Server.internal, outbus, fadeTime);
		synth.postln;
		synth.notNil.if({Server.internal.scope(numChannels, outbus); ^synth;});
	}
}