MixedBundle {
	
	var <server, <messages, <functions;
	
	*new { arg server; ^super.new.minit(server) }
	
	minit { arg srv;
		server = srv;
		messages = LinkedList.new;
		functions = LinkedList.new;
	}
	
	send { arg latency;
		server.listSendBundle(latency, messages);
		functions.do({ arg item; item.value(latency) });
	}
	
	add { arg msg;
		messages.add(msg);
	}

	addFunction { arg func;
		functions.add(func);
	}

	printOn { arg stream;
		stream << this.class.name << ": " <<< messages;
	}
	
	storeOn { arg stream;//for now
		stream << this.class.name << ": " <<< messages;
	}
}