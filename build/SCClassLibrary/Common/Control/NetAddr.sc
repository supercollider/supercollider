
NetAddr {
	var <addr=0, <>port=0, <hostname, <socket = -1;
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

	ip {
		^addr.asIPString
	}
}
