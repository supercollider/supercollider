
OSCBundle {
	var <messages, <preparationMessages;
	classvar <>safeSizes = false;
	
	add { arg msg; messages = messages.add(msg) }
	addAll { arg mgs; messages = messages.addAll(mgs) }
	addPrepare { arg msg; preparationMessages = preparationMessages.add(msg) }
	
	// the sound starts at: (next beat + latency) when the preparation is finished 
	// the client side task starts at: preparationTime + next beat
	// next beat is 0 if no clock is passed in.
	// eventstreams e.g. take into account the latency internally
	
	schedSend { arg server, clock, quant;
			this.doPrepare(server, {
				if(clock.isNil) {
						this.prSend(server, server.latency) 
				} {
						clock.schedAbs(quant.nextTimeOnGrid(clock),  {
							this.prSend(server, server.latency);  
						});
				};
			});
	}
	
	send { arg server, time; 
		this.doPrepare(server, { this.prSend(server, time) }) 
	}
	
	sendAtTime { arg server, atTime, timeOfRequest; // offset by preparation
		atTime.schedCXBundle(this,server,timeOfRequest ?? { Main.elapsedTime});
	}
	
	doPrepare { arg server, onComplete;
		if(preparationMessages.isNil) { ^onComplete.value };
		Routine.run {
			var packetSize = 16, bundle;
			if(safeSizes) {
				bundle = [];
				preparationMessages.do { |msg|
					var msgSize = msg.msgSize; 
										// this is not exactly true, as we'll pack it in a 
										// bundle later.
										// 8172 = 8192 - 16 - 4:
					if(msgSize >= 8172) { "Preparation message too big to send via UDP".error };
					if(packetSize + msgSize > 8192) { 
						server.sync(Condition.new, bundle);
						packetSize = 16;
						bundle = [];
					};
					bundle = bundle.add(msg);
					packetSize = packetSize + msgSize + 4;
				};
				if(packetSize > 0) { // send last one
					server.sync(Condition.new, bundle);
				}
			} {
				server.sync(Condition.new,preparationMessages)
			};
			onComplete.value;
		};
	}

	
	// private //
	
	prSend { arg server, latency;
		if(messages.notNil, {
			server.listSendBundle(latency, messages);
		})
	}
	

}

MixedBundle : OSCBundle {
	
	var <earlyFunctions; 	// functions to evaluate on send 
	var <functions; 		// functions to evaluate when the bundle is scheduled on the server
	
	// proposed new interface:
	
	// earlyFunctions / functions could be renamed to 
	// sendFunctions / arrivalFunctions
	
	doOnSend { arg func;
		earlyFunctions = earlyFunctions.add(func);
	}
	doOnArrival { arg func;
		functions = functions.add(func);
	}
	sched { arg time=0.0, func;
		earlyFunctions = earlyFunctions.add({ SystemClock.sched(time, { func.value; nil }) });
	}
	
	valueSend {
		functions.do({ arg item; item.value }); 
	}
	
	valueArrive {
		earlyFunctions.do({ arg item; item.value }); 
	}

	// original interface
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	addSchedFunction { arg func, time=0.0;
		earlyFunctions = earlyFunctions.add({ SystemClock.sched(time, { func.value; nil }) });
	}
	addEarlyFunction { arg func;
		earlyFunctions = earlyFunctions.add(func);
	}
	addMessage { arg receiver, selector, args; 
		functions = functions.add( Message(receiver,selector,args) )
	}
	addEarlyMessage { arg receiver, selector, args;
		earlyFunctions = earlyFunctions.add( Message(receiver,selector,args) )
	}
	
	
	doFunctions {
		functions.do({ arg item; item.value }); 
	}
	doEarlyFunctions {
		earlyFunctions.do({ arg item; item.value }); 
	}
	
	// private //
	
	prSend { arg server, latency;
		this.doEarlyFunctions;
		if(functions.notNil) {
			SystemClock.sched(latency, { this.doFunctions });
		};
		if(messages.notNil) {
			server.listSendBundle(latency, messages);
		};
	}
	
}

