+Function {

	send { arg key, server, mix=false, onComplete, loadLatency;
		var proxy;
		server = server ? Server.local;
		proxy = Ndef.new(key, nil, server);
		proxy.put(this, 0, true, mix.not, onComplete, loadLatency);
		^proxy
	}
	
	sendAdd { arg key, server;
		^this.send(key, server, true)
	}
	
	schedToBeat { arg clock, beats=0;
			clock = clock ? TempoClock.default;
			clock.schedAbs(clock.elapsedBeats.ceil + beats, 
				{ arg beat, sec, clock; this.value(beat, sec, clock); nil }
			)
	}

}

+Symbol {
	play { arg outBus=0, nChan=2, server;
		var proxy;
		server = server ? Server.local;
		proxy = Ndef.new(this, nil, server).play(outBus, nChan);
	}


}

