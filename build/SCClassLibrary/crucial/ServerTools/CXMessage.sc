

CXBundle {
	var <>functions,<>messages;
	
	add { arg message;
		messages = messages.add(message);
	}
	addFunction { arg f; // time should be the same
		functions = functions.add(f);
	}
	addMessage { arg receiver,selector, args;
		functions = functions.add( Message(receiver,selector,args) )
	}
	send { arg server,time;
		server.listSendBundle(time,messages);
		if(functions.notNil,{
			SystemClock.sched(time,{
				functions.do({ arg f; f.value });
				nil
			})
		});
	}
	clumpedSendNow { arg server;
		if(messages.notNil,{
			messages.clump(5).do({ arg bundle,i;
				server.listSendBundle(i * 0.001,bundle);
			});
			^messages.size
		},{
			^0
		})
	}
	//cancellable
}
