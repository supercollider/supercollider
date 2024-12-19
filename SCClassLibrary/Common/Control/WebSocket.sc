WebSocketServer {
    var <hostname;
    var <port;
    var <>onConnection;

    var socket; // holds the pointer to our websocket
    var connections;

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
        _WebSocketServer_Stop
        ^this.primitiveFailed;
    }

    // called from C++
    *prNewConnection {|port, newConnection|
        var webSocketServer = all[port];
        WebSocketConnection.all[newConnection.beastSessionPtr] = newConnection;
        if(webSocketServer.notNil, {
            webSocketServer.onConnection.value(newConnection);
        });
    }
}

// this gets initiated from c++ - do not create this yourself
WebSocketConnection {
    var <>beastSessionPtr; // do not modify!
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
        var connection = all.values[0]; // @todo fix this all[ptr];
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

    sendMessage {|msg|
        _WebSocketConnection_SendMessage
        ^this.primitiveFailed;
    }

    disconnect {
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
            OSCFunc.defaultDispatcher.value(m, thisProcess.tick, ~net, 57120);
            OSCFunc.defaultMatchingDispatcher.value(m, thisProcess.tick, ~net, 57120);
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
