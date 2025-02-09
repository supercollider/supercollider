WebSocketServer {
    var <hostname;
    var <port;
    var <>socket; // @todo make this private - holds the pointer to our websocket
    var <>running = false; // @todo make this private
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
        if (running) {
            "WebSocketServer already started".warn;
            ^this;
        };
        this.prStart;
        running = true;
    }

    prStart {
        _WebSocketServer_Start
        ^this.primitiveFailed;
    }

	stop {
		if (running.not) {
		    "Can only stop a running server".warn;
		    ^this;
		};
		connections.do({|connection|
			connection.close;
		});
		this.prStop;
		running = false;
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
    var <beastSessionPtr;
	var <>connected = true; // do not modify!
    var <>onMessage; // set this as a callback
    var <>onDisconnect; // set this as a callback

    classvar <>all;

    *initClass {
        all = ();
    }

    // this gets called from c++
    *prReceiveMessage { |ptr, message|
        var connection = all[ptr];
        if(connection.notNil, {
            connection.onMessage.value(message);
        });
    }

    *prDisconnect { |ptr|
        var connection = all[ptr];
        if(connection.notNil, {
            connection.onDisconnect.value();
            connection.connected = false;
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
        if(connected.not, {
            "Can not send a message on a closed connection".warn;
            ^this;
        });
        if(msg.isKindOf(String), {
            ^this.prSendStringMessage(msg);
        });
        if(msg.isKindOf(Int8Array), {
            ^this.prSendRawMessage(msg);
        });
        "Message of type '%' is unsupported".format(msg.class).warn;
    }

	close {
		if(connected, {
			this.prClose();
		});
	}

    prClose {
        _WebSocketConnection_Close
        ^this.primitiveFailed;
    }
}

WebSocketClient {
    var <host;
    var <port;
    var <beastConnectionPtr;
    var <>connected = false; // set via `prSetConnectionStatus` callback
    var <>onMessage;
    var <>onDisconnect;

    classvar <>globalConnections;

    *initClass {
        globalConnections = ();
    }

    *new {|host="127.0.0.1", port=8765|
        ^super.newCopyArgs(host, port);
    }

    *prReceivedMessage {|ptr, message|
        var connection = globalConnections[ptr];
        if(connection.notNil, {
            connection.onMessage.value(message);
        });
    }

    *prSetConnectionStatus {|ptr, connected|
        var connection = globalConnections[ptr];
        if(connection.notNil, {
            if(connected, {
                connection.connected = true;
            }, {
                connection.onDisconnect.value();
                connection.connected = false;
            });
        });
    }

    connect {
        this.prConnect;
        globalConnections[beastConnectionPtr] = this;
    }

    *prConnected {|ptr|
        var client = globalConnections[ptr];
        if(client.notNil, {
            client.connected = true;
        });
    }

    prConnect {
        _WebSocketClient_Connect
        ^this.primitiveFailed;
    }

    close {
        if(beastConnectionPtr.isNil, {
            "Connection is not available - can not close.".warn;
            ^this;
        });
        if(connected.not, {
            "Can only close an open connection".warn;
            ^this;
        });

        this.prClose;
        connected = false;
        globalConnections[beastConnectionPtr] = nil;
    }

    // @todo check if ptr is set!
    prClose {
        _WebSocketClient_Close
        ^this.primitiveFailed;
    }

    sendMessage {|msg|
        if(connected.not, {
            "Can only send a message on an open connection".warn;
            ^this;
        });
        if(msg.isKindOf(String), {
            ^this.prSendStringMessage(msg);
        });
        if(msg.isKindOf(Int8Array), {
            ^this.prSendRawMessage(msg);
        });
        "Unknown datatype %".format(msg.class).warn;
    }

    prSendStringMessage {|msg|
        _WebSocketClient_SendStringMessage
        ^this.primitiveFailed;
    }

    prSendRawMessage {|msg|
        _WebSocketClient_SendRawMessage
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
