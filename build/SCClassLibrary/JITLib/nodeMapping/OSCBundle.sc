
OSCBundle {
	var <messages;
		
	send { arg server, time;
		server.listSendBundle(time, messages);
	}
	schedSend { arg server, time, clock;
		(clock ? SystemClock).sched(time ? 0,{ this.send(server,time) });
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
	the latency is determined from the server.
	the time arg is the time between prepare and send (schedSend) or 
	the sched time from the reception of the '/done' message (schedSendRespond) 

*/

MixedBundle : OSCBundle {
	
	var <functions; //functions to evaluate on send 
	var <preparationMessages; //messages to send first on schedSend
	
	
	
	schedSend { arg server, time, clock, onCompletion;
			this.sendPrepare(server);
			(clock ? SystemClock).sched(time ? 0, {
								this.send(server);
								onCompletion.value 
			});
		
	}
	
	schedSendRespond { arg server, time, clock, onCompletion;
		var resp;
			
			resp = OSCresponderNode(server.addr, '/done', { 
							(clock ? SystemClock).sched(time ? 0,{ 
								this.send(server);
								onCompletion.value
							});
							resp.remove;
				}).add;
			this.sendPrepare(server);
	}
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	
	addPrepare { arg msg;
		preparationMessages = preparationMessages.add(msg);
	}
	
	send { arg server, latency;
		var array;
		latency = latency ? server.latency;
		functions.do({ arg item; item.value(latency) });
		//messages.asCompileString.postln;
		server.listSendBundle(latency, messages);
	}
	
	sendPrepare { arg server;
		server.listSendBundle(nil, preparationMessages);
	}

		
}

