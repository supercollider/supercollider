
OSCBundle {
	var <messages, <preparationMessages; //messages to send first on schedSend
	
	add { arg msg; messages = messages.add(msg) }
	addAll { arg mgs; messages = messages.addAll(mgs) }
	addPrepare { arg msg; preparationMessages = preparationMessages.add(msg) }
	
	// the sound starts at: (next beat + latency) when the preparation is finished 
	// the client side task starts at: preparationTime + next beat
	// next beat is 0 if no clock is passed in.
	// eventstreams e.g. take into account the latency internally
	
	schedSend { arg server, clock, quant=1.0;
			Routine.run {
				if(preparationMessages.notNil) {
					server.sync(Condition.new, preparationMessages);
				};
				if(clock.isNil, {
						this.prSend(server, server.latency) 
				}, {
						clock.schedAbs(quant.nextTimeOnGrid(clock), {
							this.prSend(server, server.latency);  
						});
				});
			};
	}
	
	send { arg server, time;
			Routine.run {
				if(preparationMessages.notNil) {
					server.sync(Condition.new, preparationMessages);
				};
				this.prSend(server, time);
			}
	}
	
	sendAtTime { arg server, atTime, timeOfRequest; // offset by preparation
		atTime.schedCXBundle(this,server,timeOfRequest ?? {Main.elapsedTime});
	}
	
	// private //
	
	prSend { arg server, latency;
		if(messages.notNil, {
			server.listSendBundle(latency, messages);
		})
	}
	


}

MixedBundle : OSCBundle {
	
	var <functions; //functions to evaluate on send 
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	
	addSchedFunction { arg func, time=0.0;
		functions = functions.add({ SystemClock.sched(time, { func.value; nil }) });
	}
	
	addMessage { arg receiver, selector, args;
		functions = functions.add( Message(receiver,selector,args) )
	}
	doFunctions {
		functions.do({ arg item; item.value }); 
	}
	
	// private //
	
	prSend { arg server, latency;
		this.doFunctions;
		if(messages.notNil, {
			server.listSendBundle(latency, messages);
		})
	}
		
	
}

