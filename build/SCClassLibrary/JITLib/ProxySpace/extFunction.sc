+Function {

	send { arg key, add=false;
		var proxy;
		proxy = Library.at(\proxy, key);
		if(proxy.isNil, {
			proxy = NodeProxy(Library.at(\defaultServer) ? Server.local);
			Library.put(\proxy, key, proxy);
		});
		proxy.setObj(this, true, add.not, add);
		^proxy
	}
	
	sendAdd { arg key;
		^this.send(key, true)
	}

}

+Symbol {
	play { arg outBus=0, nChan=2;
		var proxy;
		proxy = Library.at(\proxy, this);
		if(proxy.isNil, {
			proxy = NodeProxy(Library.at(\defaultServer) ? Server.local);
			Library.put(\proxy, this, proxy);
		});
		^proxy.play(outBus, nChan);
	}


}