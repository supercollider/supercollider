+Function {

	send { arg key, server, freeAll=true, onComplete, latency;
		var proxy;
		server = server ? Server.local;
		proxy = Library.at(\proxy, server, key);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			Library.put(\proxy, server, key, proxy);
		});
		proxy.setObj(this, true, freeAll, onComplete, latency);
		^proxy
	}
	
	sendAdd { arg key, server;
		^this.send(key, server, true)
	}

}

+Symbol {
	play { arg outBus=0, nChan=2, server;
		var proxy;
		server = server ? Server.local;
		proxy = Library.at(\proxy, server, this);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			Library.put(\proxy, server, this, proxy);
		});
		^proxy.play(outBus, nChan);
	}


}