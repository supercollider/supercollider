
/*
	this bundle is offset by preparationTime at all times.

*/

MixedBundle {
	
	var <messages, <functions; //functions to evaluate on send 
	var <preparationMessages; //messages to send first on schedSend
	
	add { arg msg;
		messages = messages.add(msg);
	}
	
	addAll { arg mgs;
		messages = messages.addAll(mgs)
	}
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	
	addSchedFunction { arg func, time=0;
		functions = functions.add({ SystemClock.sched(time, { func.value; nil }) });
	}
	
	addPrepare { arg msg;
		preparationMessages = preparationMessages.add(msg);
	}
	
	addAction { arg receiver, selector, args;
		functions = functions.add( Message(receiver,selector,args) )
	}
	doFunctions {
		functions.do({ arg item; item.value }); 
	}
	
	sendPrepare { arg server; server.listSendBundle(nil, preparationMessages) }
	
	sendAndEvaluate { arg server, latency;
		var array; // maybe pass in latency? does not work for perform message
		this.doFunctions;
		if(messages.notNil, {
			server.listSendBundle(latency, messages);
		})
	}
	
	// the sound starts at: (next beat + latency) when the preparation is finished 
	// the client side task starts at: preparationTime + next beat
	// next beat is 0 if no clock is passed in.
	// eventstreams e.g. take into account the latency internally
	
	schedSend { arg server, clock;
			Routine.run {
				if(preparationMessages.notNil) {
					this.sendPrepare(server);
					server.sync;
				};
				if(clock.isNil, {
						this.sendAndEvaluate(server, server.latency) 
				}, {
						clock.schedAbs(clock.elapsedBeats.ceil, { 
							this.sendAndEvaluate(server, server.latency);  
						});
				});
			};
	}
	
	send { arg server, time;
			Routine.run {
				if(preparationMessages.notNil) {
					this.sendPrepare(server);
					server.sync;
				};
				this.sendAndEvaluate(server, time);
			}
	}
	
	// offset by preparation time.
	
	sendAtTime { arg server, atTime, timeOfRequest;
		atTime.schedCXBundle(this,server,timeOfRequest ?? {Main.elapsedTime});
	}
	
	printOn { arg stream;
		stream << this.class.name << ": " <<< messages;
	}
	
	storeOn { arg stream;//for now
		stream << this.class.name << ": " <<< messages;
	}
	
}

DebugBundle : MixedBundle {
	sendAndEvaluate { arg server, latency;
		var array;
		functions.do({ arg item; item.value(latency, server) }); 
		"sending messages:".inform;
		"[".postln;
		messages.do({ arg item; item.asCompileString.postln });
		"]".postln;
		server.listSendBundle(latency, messages);
	}
	
	sendPrepare { arg server;
		"sending preparation messages:".inform;
		preparationMessages.asCompileString.postln; 
		Char.nl.post;
		server.listSendBundle(nil, preparationMessages);
	}


}