
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
		server = server ?? { Server.default };
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
		server = server ?? { Server.default };
		this.doPrepare(server, { this.prSend(server, time) }) 
	}
	
	sendAtTime { arg server, atTime, timeOfRequest; // offset by preparation
		server = server ?? { Server.default };
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
										// 65515 = 65535 - 16 - 4:
					if(msgSize >= 65515) { "Preparation message too big to send via UDP".error };
					if(packetSize + msgSize > 65535) { 
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
			server.listSendBundle(latency ?? { server.latency }, messages);
		})
	}
	

}

MixedBundle : OSCBundle {
	
	var <sendFunctions; 	// functions to evaluate on send 
	var <functions; 		// functions to evaluate when the bundle is scheduled on the server
	
	
	onSend { arg func;
		sendFunctions = sendFunctions.add(func);
	}
	addFunction { arg func;
		functions = functions.add(func);
	}
	sched { arg time=0.0, func;
		sendFunctions = sendFunctions.add({ SystemClock.sched(time, { func.value; nil }) });
	}
	
	addMessage { arg receiver, selector, args; 
		functions = functions.add( Message(receiver,selector,args) )
	}
	addOnSendMessage { arg receiver, selector, args;
		sendFunctions = sendFunctions.add( Message(receiver,selector,args) )
	}
	
	
	doFunctions {
		functions.do({ arg item; item.value }); 
	}
	doSendFunctions {
		sendFunctions.do({ arg item; item.value }); 
	}
	
	// private //
	
	prSend { arg server, latency;
		latency = latency ?? { server.latency };
		if(functions.notNil) {
			SystemClock.sched(latency, { this.doFunctions });
		};
		if(messages.notNil) {
			server.listSendBundle(latency, messages);
		};
		this.doSendFunctions;
	}
	
}

