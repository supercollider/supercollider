
NetAddr {
	var <addr=0, <>port=0, <hostname, <socket;
	classvar connections;

	*initClass {
		UI.registerForShutdown({
			this.disconnectAll
		});
	}

	*new { arg hostname, port=0;
		var addr;
		addr = if (hostname.notNil,{ hostname.gethostbyname },{ 0 });
		^super.newCopyArgs(addr, port, hostname);
	}
	*fromIP { arg addr, port=0;
		^super.newCopyArgs(addr, port, addr.asIPString)
	}

	*useDoubles_ { arg flag = false;
		_NetAddr_UseDoubles
		^this.primitiveFailed;
	}
	*disconnectAll {
		connections.copy.do({ | netAddr |
			netAddr.disconnect;
		});
		connections = nil;
	}

	hostname_ { arg inHostname;
		hostname = inHostname;
		addr = inHostname.gethostbyname;
	}
	

	sendRaw { arg rawArray;
		_NetAddr_SendRaw
		^this.primitiveFailed;
	}
	sendMsg { arg ... args;
		_NetAddr_SendMsg
		^this.primitiveFailed;
	}
	sendBundle { arg time ... args;
		_NetAddr_SendBundle
		^this.primitiveFailed;
	}
	
	isConnected {
		^socket.notNil
	}
	connect {
		if (this.isConnected.not) {
			this.prConnect;
			connections = connections.add(this);
		};
	}
	disconnect {
		if (this.isConnected) {
			this.prDisconnect;
		};
	}
	
	== { arg that; 
		^(this.port == that.port) and: { this.addr == that.addr }
	}
	hash { arg that;
		^addr.hash bitXor: port.hash
	} 

	ip {
		^addr.asIPString
	}

	printOn { | stream |
		super.printOn(stream);
		stream << $( << this.ip << ", " << port << $)
	}

	// PRIVATE
	prConnect {
		_NetAddr_Connect
		^this.primitiveFailed;		
	}
	prDisconnect {
		_NetAddr_Disconnect
		^this.primitiveFailed;
	}
	prConnectionClosed {
		// called from the recv thread when connection is closed
		// either by sclang or by peer
		if (connections.notNil) {
			connections.remove(this);
		};
	}
}
