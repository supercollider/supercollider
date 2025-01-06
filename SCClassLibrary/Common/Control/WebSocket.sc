WebSocketServer {
    var <hostname;
    var <port;
    var <>socket; // holds the pointer to our websocket
    var <>onConnection;
    var <>connections;

    classvar <>all;

    *initClass {
        all = ();
    }

    *new {|hostname="127.0.0.1", port=8080|
        var res = super.newCopyArgs(
            hostname,
            port,
        );
        all[port] = res;
        ^res;
    }

    start {
        _WebSocketServer_Start
        ^this.primitiveFailed;
    }

	stop {
		connections.do({|connection|
			connection.close;
		});
		this.prStop;
	}

    prStop {
        _WebSocketServer_Stop
        ^this.primitiveFailed;
    }

    // called from C++
    *prNewConnection {|port, newConnection|
        var webSocketServer = all[port];
        WebSocketConnection.all[newConnection.beastSessionPtr] = newConnection;
        if(webSocketServer.notNil, {
            webSocketServer.connections = webSocketServer.connections.add(newConnection);
            webSocketServer.onConnection.value(newConnection);
        });
    }
}

// this gets initiated from c++ - do not create this yourself
WebSocketConnection {
    var <>beastSessionPtr; // do not modify!
	var <>closed = false; // also managed by c++
    var <>onMessage; // set this as a callback
    var <>onDisconnect; // set this as a callback

    // @todo how can we remove the setter?
    // b/c we don't want to call .init from within c++
    classvar <>all;

    *initClass {
        all = ();
    }

    // this gets called from c++
    *prReceiveMessage { |ptr, message|
        var connection = all[ptr]; // @todo fix this all[ptr];
        if(connection.notNil, {
            connection.onMessage.value(message);
        });
    }

    *prConnectionDisconnect { |ptr|
        var connection = all[ptr];
        if(connection.notNil, {
            connection.onDisconnect.();
        });
        all[ptr] = nil;
    }

    prSendRawMessage {|msg|
        _WebSocketConnection_SendRawMessage
        ^this.primitiveFailed;
    }

    prSendStringMessage {|msg|
        _WebSocketConnection_SendStringMessage
        ^this.primitiveFailed;
    }

    sendMessage {|msg|
        if(closed, {
            "Can not send a message on a closed connection".warn;
            ^this;
        });
        if(msg.isKindOf(String), {
            ^this.prSendStringMessage(msg);
        });
        if(msg.isKindOf(Int8Array), {
            ^this.prSendRawMessage(msg);
        });
        "Message type '%' is unsupported".format(msg.class).warn;
    }

	close {
		if(closed.not, {
			this.prClose;
		});
	}

    prClose {
        _WebSocketConnection_Close
        ^this.primitiveFailed;
    }
}

WebSocketNetAddr : NetAddr {
	var <>webSocketConnection;
	var <>debug;
	var <>onMessage;

	*new {|webSocketConnection|
		^super.newCopyArgs().init(webSocketConnection);
	}

	init {|connection|
	    addr = 0; // Server needs this initialized
	    debug = false;
	    webSocketConnection = connection;
	    webSocketConnection.onMessage = {|m|
            m = Int8Array.newFrom(m).parseOSC;
			thisProcess.recvOSCmessage(
				time: thisProcess.tick, // @todo really?
				replyAddr: this,
				recvPort: NetAddr.langPort,
				msg: m,
			);
			// OSCFunc.defaultDispatcher.value(m, thisProcess.tick, ~net, 57120);
			// OSCFunc.defaultMatchingDispatcher.value(m, thisProcess.tick, ~net, 57120);
	    };
	}

	sendRaw {|rawArray|
	    if(debug, {["raw", rawArray].postln;});
		webSocketConnection.sendMessage(rawArray);
	}

	sendMsg {|...content|
		this.sendRaw(content.asRawOSC);
	}

	sendBundle {|time ...messages|
	    ["bundle not supported?", time, messages].postln;
	    messages.do({|message|
	        this.sendRaw(message.asRawOSC);
	    });
	}
}
