
NetAddr {
	var <addr=0, <>port=0, <hostname, <socket = -1;
	*new { arg hostname, port=0;
		var addr;
		addr = if (hostname.notNil, { hostname.gethostbyname },{0});
		^super.newCopyArgs(addr, port, hostname);
	}
	hostname_ { arg inHostname;
		hostname = inHostname;
		addr = inHostname.gethostbyname;
	}
	
	send { arg oscAddress ... args;
		_OSC_Send
		^this.primitiveFailed;
	}
	sendNoTags { arg oscAddress ... args;
		_OSC_SendNoTags
		^this.primitiveFailed;
	}
	
	sendArray { arg array;
		this.performList(\send, array);
	}

///////////////

	sendRaw { arg rawArray;
		_NetAddr_SendRaw
		^this.primitiveFailed;
	}
	sendMsg { arg ... args;
		this.sendBundle(nil, args);
	}
	sendBundle { arg time ... args;
		_NetAddr_SendBundle
		^this.primitiveFailed;
	}
	
	connect {
		_NetAddr_Connect
		^this.primitiveFailed;
	}
	disconnect {
		_NetAddr_Disconnect
		^this.primitiveFailed;
	}
	
	== { arg that; 
		^(this.port == that.port) and: { this.addr == that.addr }
	}
	hash { arg that;
		^addr.hash bitXor: port.hash
	} 
}
