
OSCBundle {
	var <server, <messages;
	*new { arg server; ^super.new.minit(server) }
	
	minit { arg srv;
		server = srv;
	}	
	send {
		server.listSendBundle(nil, messages);
	}
	schedSend { arg time, clock;
		(clock ? SystemClock).sched(time ? 0,{ this.send });
	}
	add { arg msg;
		messages = messages.add(msg);
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
	
	minit { arg srv;
		server = srv;
	}
	
	schedSend { arg time, clock, onCompletion;
		//maybe use OSCScheduler later
			this.sendPrepare;
			(clock ? SystemClock).sched(time ? 0, {
								this.send;
								onCompletion.value 
			});
		
	}
	
	schedSendRespond { arg time, clock, onCompletion;
		var resp;
			
			resp = OSCresponder(server.addr, '/done', { 
							(clock ? SystemClock).sched(time ? 0,{ 
								this.send;
								onCompletion.value
							});
							resp.remove;
				}).add;
			this.sendPrepare;
	}
	
	addFunction { arg func;
		functions = functions.add(func);
	}
	
	addPrepare { arg msg;
		preparationMessages = preparationMessages.add(msg);
	}
	
	send { 
		var array, latency;
		//array = asRawOSC([nil,messages]).postln;
		latency = server.latency;
		functions.do({ arg item; item.value(latency) });
		server.listSendBundle(latency, messages);
		//server.sendRaw(array);
		
	}
	
	sendPrepare {
		server.listSendBundle(nil, preparationMessages);
	}

		
}

