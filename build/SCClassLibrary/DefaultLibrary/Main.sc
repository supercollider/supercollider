Main : Process {
	
	startUp {
		super.startUp;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.local;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
	}

	run {
	}
	stop {
		SystemClock.clear;
		AppClock.clear;
	}
	
	recvOSCmessage { arg time, replyAddr, msg;
		// this method is called when an OSC message is received.
		OSCresponder.respond(time, replyAddr, msg);
	}
	
	recvOSCbundle { arg time, replyAddr ... msgs;
		// this method is called when an OSC bundle is received.
		msgs.do({ arg msg; 
			this.recvOSCmessage(time, replyAddr, msg); 
		});
	}
	
}


