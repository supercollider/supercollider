
CXBundle {
	var <>functions,<>messages;
	
	add { arg message;
		messages = messages.add(message);
	}
	addFunction { arg f;
		functions = functions.add(f);
	}
	addAction { arg receiver,selector, args;
		functions = functions.add( Message(receiver,selector,args) )
	}
	send { arg server,time;
		//messages.flat.size.debug("messages size");
		server.listSendBundle(time,messages);
		if(functions.notNil,{
			if(time.notNil,{
				SystemClock.sched(time,{
					this.doFunctions;
					nil
				})
			},{
				this.doFunctions;
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
	doFunctions {
		functions.do({ arg f; f.value });
	}
	//cancellable
}
