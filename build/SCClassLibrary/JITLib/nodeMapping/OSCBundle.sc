
OSCBundle {
	var <messages;
		
	send { arg server, latency;
		server.listSendBundle(latency, messages);
	}
	schedSend { arg server, time, clock, latency;
		SystemClock.sched(time ? 0, {
			if(clock.isNil, {
				this.send(server, latency) 
			}, {
				clock.schedAbs(clock.elapsedBeats.ceil, { this.send(server) })
			});
		})
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
	sendPrepare: on schedSend this is done before the other messages are sent
	send: the functions evaluated, if there is any and the messages are sent.
	
	the time arg is the time between prepare and send (schedSend) or 
	the sched time from the reception of the '/done' message (schedSendRespond) 

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
	
	send { arg server, latency;
		var array;
		//no latency here, because usuallay the streams etc have their own 
		//latency handling:
		functions.do({ arg item; item.value(latency, server) }); 
		server.listSendBundle(latency, messages);
	}
	
	sendPrepare { arg server, latency;
		server.listSendBundle(latency, preparationMessages);
	}
	
	schedSend { arg server, clock;
			this.sendPrepare(server, server.latency);
			SystemClock.sched(preparationTime, {
				if(clock.isNil, {
					this.send(server, server.latency) 
				}, {
					clock.schedAbs(clock.elapsedBeats.ceil, { 
						this.send(server, server.latency);  
					});
				});
				nil
			})
		
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
	send { arg server, latency;
		var array;
		functions.do({ arg item; item.value(latency, server) }); 
		"sending messages:".inform;
		"[".postln;
		messages.do({ arg item; item.asCompileString.postln });
		"]".postln;
		server.listSendBundle(latency, messages);
	}
	
	sendPrepare { arg server, latency;
		"sending preparation messages:".inform;
		preparationMessages.asCompileString.postln; 
		("current delay between preparation and action:" + preparationTime).inform;
		Char.nl.post;
		server.listSendBundle(latency, preparationMessages);
	}


}