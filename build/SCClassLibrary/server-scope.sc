+ Server {

	scope {arg numChannels, startingChannel = 0, bufsize = 4096;
		var buf, numChan, window, scope, synth;
		if((this === Server.internal), {
			numChan = numChannels ? this.options.numOutputBusChannels;
			buf = Buffer.alloc(this, bufsize, numChan, { 
				arg buffer;
				window = SCWindow("Internal Server", Rect(64, 64, 660, 280));
				scope = SCScope(window, Rect(10,10,640,256));
				scope.bufnum = buffer.bufnum;
				scope.background = Color.black;
				scope.resize = 5;
				synth = SynthDef("server-scope",{ 
					ScopeOut.ar(InFeedback.ar(startingChannel, numChan), buffer.bufnum);  
				}).play(this, addAction: \addToTail);
				// free synth and buffer when window is closed
				window.onClose = { synth.free; buffer.free; };
				window.front;
				// uncomment the following?
				// ^synth;
			})
		
		}, {"Server-scope only works with the Internal Server".warn});
	}
}