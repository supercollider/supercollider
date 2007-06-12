
CXBundle {

	var <>functions,<>messages;
	
	add { arg message;
		messages = messages.add(message);
	}
	addFunction { arg f;
		functions = functions.add(f);
	}
	addMessage { arg receiver,selector, args;
		functions = functions.add( Message(receiver,selector,args) )
	}
	sendAtTime { arg server,atTime,timeOfRequest;
		atTime.schedCXBundle(this,server,timeOfRequest ?? {Main.elapsedTime});
	}
	send { arg server,delta;
		if(messages.notNil,{
			server.listSendBundle(delta,messages);
		});
		if(functions.notNil,{
			if(delta.notNil and: {delta > 0.0},{
				SystemClock.sched(delta,{
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
			messages.clump(10).do({ arg bundle,i;
				server.listSendBundle(i * 0.001,bundle);
			});
			SystemClock.sched(messages.size * 0.001,{ this.doFunctions; nil });
			^messages.size
		},{
			this.doFunctions;
			^0
		})
	}
	doFunctions {
		functions.do({ arg f; f.value });
	}
}




