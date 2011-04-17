OSCBundle {
	var <messages, <preparationMessages;

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
			if(quant.isNil or: { clock.isNil }) {
				this.prSend(server, server.latency)
			} {
				clock.schedAbs(quant.nextTimeOnGrid(clock),  {
					this.prSend(server, server.latency);
				});
			};
		});
	}

	send { arg server, time, timeOfRequest;
		server = server ?? { Server.default };
		timeOfRequest = timeOfRequest ?? {Main.elapsedTime};
		this.doPrepare(server, { this.prSend(server, time,timeOfRequest) })
	}
	// see atTime helpfile
	sendAtTime { arg server, atTime, timeOfRequest; // offset by preparation
		server = server ?? { Server.default };
		atTime.schedBundle(this,server,timeOfRequest ?? { Main.elapsedTime});
	}

	doPrepare { arg server, onComplete;
		if(preparationMessages.isNil) { ^onComplete.value };

		Routine.run {
			server.sync(Condition.new, preparationMessages);
			onComplete.value;
		};
	}


	// private //

	prSend { arg server, delta,timeOfRequest;
		if(messages.notNil, {
			server.listSendBundle(delta ?? { server.latency }, messages);
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

	prSend { arg server, delta, timeOfRequest;
		if(delta.isNil,{
			delta = server.latency;
		},{
			if(timeOfRequest.notNil,{
				delta = (timeOfRequest + delta) - Main.elapsedTime;
				if(delta.isNegative,{
					("bundle sent late:" + delta).warn;
					delta = 0.0;// just send it now
				});
			});
		});
		if(functions.notNil) {
			SystemClock.sched(delta ? 0.0, {
				this.doFunctions;
			});
		};
		if(messages.notNil) {
			server.listSendBundle(delta, messages);
		};
		this.doSendFunctions;
	}

}
