
OSCBundle {
	var <messages;
		
	send { arg server, delta;
		if(messages.notNil,{
			server.listSendBundle(delta, messages);
		});
	}
	
	add { arg msg;
		messages = messages.add(msg);
	}
	addAll { arg mgs;
		messages = messages.addAll(mgs)
	}
	
	printOn { arg stream;
		stream << this.class.name << ": " <<< messages;
	}
	
	storeOn { arg stream;//for now
		stream << this.class.name << ": " <<< messages;
	}
}

/*
	this bundle is offset by preparationTime at all times.

*/

MixedBundle : OSCBundle {
	
	var <functions; //functions to evaluate on send 
	var <preparationMessages; //messages to send first on schedSend
	var <>preparationTime=0.3; //time between preparation and action
	
	addFunction { arg func;
		functions = functions.add(func);
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
	
	sendPrepare { arg server;
		if(preparationMessages.notNil, {
			server.listSendBundle(nil, preparationMessages);
		})
	}
	
	sendAndEvaluate { arg server, latency;
		var array; // maybe pass in latency? does not work for perform message
		this.doFunctions;
		if(messages.notNil, {
			server.listSendBundle(latency, messages);
		})
	}
	
	// the sound starts at: preparationTime + (next beat + latency) 
	// the client side task starts at: preparationTime + next beat
	// next beat is 0 if no clock is passed in.
	// eventstreams e.g. take into account the latency internally
	
	schedSend { arg server, clock;
			this.sendPrepare(server);
			SystemClock.sched(preparationTime, {
				if(clock.isNil, {
					this.sendAndEvaluate(server, server.latency) 
				}, {
					clock.schedAbs(clock.elapsedBeats.ceil, { 
						this.sendAndEvaluate(server, server.latency);  
					});
				});
				nil
			})
		
	}
	
	send { arg server, time;
			this.sendPrepare(server);
			SystemClock.sched(preparationTime, {
				this.sendAndEvaluate(server, server.latency);
				nil
			})
	}
	
	// offset by preparation time.
	sendAtTime { arg server, atTime, timeOfRequest;
		atTime.schedCXBundle(this,server,timeOfRequest ?? {Main.elapsedTime});
	}
	
	
	//need to wait for id in async messages
	//schedSendRespond { arg server, clock;
//			var msg, prep, func;
//			func = {
//								if(clock.isNil, {
//										this.send(server, server.latency) 
//									}, {
//										clock.schedAbs(clock.elapsedBeats.ceil, { 
//											this.send(server, server.latency); 
//											nil 
//									})
//							});
//				};
//			if(preparationMessages.notNil, {
//				msg = AsyncResponder.nextMsg(server, func);
//				prep = preparationMessages.last.add(msg);
//				preparationMessages.put(preparationMessages.size-1, prep).debug;				this.sendPrepare(server, server.latency);
//			}, func);
//	}
	//todo
	//schedSendRespond { arg server, clock;
//			var resp;
//			resp = OSCresponderNode(server.addr, '/done', {
//								if(clock.isNil, {
//										this.send(server, server.latency) 
//									}, {
//										clock.schedAbs(clock.elapsedBeats.ceil, { 
//											this.send(server, server.latency); 
//											nil 
//									})
//							});
//							resp.remove;
//				}).add;
//			this.sendPrepare(server, server.latency);
//	}
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
		("current delay between preparation and action:" + preparationTime).inform;
		Char.nl.post;
		server.listSendBundle(nil, preparationMessages);
	}


}