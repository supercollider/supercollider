
OSCBundle {
	var <server, <messages;
	*new { arg server; ^super.new.minit(server) }
	
	minit { arg srv;
		server = srv;
		messages = Array.new;
	}	
	send {
		server.listSendBundle(nil, messages);
	}
	schedSend { arg time, clock;
		(clock ? SystemClock).sched(time,{ this.send });
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

MixedBundle : OSCBundle {
	
	var <functions; //functions to evaluate on send 
	var <preparationMessages; //messages to send first on schedSend
	
	minit { arg srv;
		server = srv;
		messages = Array.new;
		functions = Array.new;
		preparationMessages = Array.new;
	}
	
	schedSend { arg time=0, clock, useResponder=false, onCompletion;
		var resp;
		//use OSCScheduler later
		if(useResponder && preparationMessages.isEmpty.not, {
			resp = OSCresponder(server.addr, '/done', { 
							(clock ? SystemClock).sched(time,{ 
								this.send;
								onCompletion.value
							});
							resp.remove;
					}).add;
			this.sendPrepare;
		},{
			this.sendPrepare;
			(clock ? SystemClock).sched(time,{
								this.send;
								onCompletion.value 
			});
		});
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
		server.listSendBundle(latency, messages);
		//server.sendRaw(array);
		functions.do({ arg item; item.value(latency) });
	}
	
	sendPrepare {
		server.listSendBundle(nil, preparationMessages);
	}

		
}