OSCBundle {
	var <server, <messages;
	*new { arg server; ^super.new.minit(server) }
	
	minit { arg srv;
		server = srv;
		messages = LinkedList.new;
	}	
	send { arg latency;
		var array;
		server.listSendBundle(latency, messages);
	}
	add { arg msg;
		messages.add(msg);
	}
	printOn { arg stream;
		stream << this.class.name << ": " <<< messages;
	}
	
	storeOn { arg stream;//for now
		stream << this.class.name << ": " <<< messages;
	}
}

MixedBundle : OSCBundle {
	
	var <functions;
	
	minit { arg srv;
		server = srv;
		messages = LinkedList.new;
		functions = LinkedList.new;
	}
	
	send { arg latency;
		var array;
		//array = asRawOSC([nil,messages]).postln;
		server.listSendBundle(latency, messages);
		//server.sendRaw(array);
		functions.do({ arg item; item.value(latency) });
	}
	
	addFunction { arg func;
		functions.add(func);
	}

	
}