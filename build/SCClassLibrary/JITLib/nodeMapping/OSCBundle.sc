
OSCBundle {
	var <messages;
		
	send { arg server, time;
		server.listSendBundle(time, messages);
	}
	schedSend { arg server, time, clock;
		SystemClock.sched(time ? 0, {
			if(clock.isNil, {
				this.send(server) 
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
	var <>preparationTime=0.2; //time between preparation and action
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	
	addPrepare { arg msg;
		preparationMessages = preparationMessages.add(msg);
	}
	
	
	send { arg server, latency;
		var array;
		latency = latency ? server.latency;
		//no latency here, because usuallay the streams etc have their own 
		//latency handling:
		functions.do({ arg item; item.value(latency, server) }); 
		server.listSendBundle(latency, messages);
	}
	
	sendPrepare { arg server, latency;
		server.listSendBundle(latency, preparationMessages);
	}
	
	schedSend { arg server, clock, onCompletion;
			this.sendPrepare(server, server.latency);
			if(clock.isNil, {
					this.send(server, server.latency + preparationTime) 
			}, {
				clock.schedAbs(clock.elapsedBeats.ceil, { 
					this.send(server, server.latency + preparationTime); 
					nil 
				});
			})
		
	}
	//todo
	schedSendRespond { arg server, clock, onCompletion;
			var resp;
			resp = OSCresponderNode(server.addr, '/done', {
								if(clock.isNil, {
										this.send(server, server.latency) 
									}, {
										clock.schedAbs(clock.elapsedBeats.ceil, { 
										this.send(server, server.latency); 
										nil 
									})
							});
							resp.remove;
				}).add;
			this.sendPrepare(server, server.latency);
	}
}

DebugBundle : MixedBundle {
	send { arg server, latency;
		var array;
		latency = latency ? server.latency;
		functions.do({ arg item; item.value(latency, server) }); 
		"sending messages:".inform;
		messages.asCompileString.postln;
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