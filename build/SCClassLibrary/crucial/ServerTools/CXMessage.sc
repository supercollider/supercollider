

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
	//cancelable
}




/*
complicated, crap

	
	b.addCXBundle(this.spawnMsg)
	
		b.addCXBundle(server,child.spawnMsg)
		child.spawnToCXBundle(b)
		
		b.addMsg(synth.addToTailMsg) 
		b.addMessage(this,\didSpawn)
		
	
	
	
	*/

//
//
//CXMessage {
//	var <>server,<>message;
//	
//	*new { arg server, message;
//		^super.newCopyArgs(server,message)
//	}
//	send {
//		server.listSendMsg(message)
//	}
//	++ { arg aMsg;
//		^aMsg.asCXBundle.add(this)
//	}
//	asCXBundle { ^CXBundle(server,this) }
//
//}
//
//CXBundle {
//	var <>server,<>messages,<>time;
//	
//	*new { arg server, messages,time;
//		^super.newCopyArgs(server,messages,time)
//	}
//	add { arg mtime,message;
//		if(mtime != time,{
//			^CXMultiBundle.new.add(this).add(CXBundle(server,[message],mtime))
//		},{
//			if(message.isKindOf(CXBundle),{
//				this.addBundle(message)
//			},{
//				messages = messages.add(message)
//			})
//		})
//	}
//	addBundle { arg bundle; // time should be the same
//		messages = messages.addAll(bundle.mssages);
//	}
//	send {
//		server.listSendBundle(time,messages.collect({ arg m; m.message }))
//	}
//	asCXBundle {}
//	++ { arg aBundle;
//		aBundle.asCXBundle.addBundle(this)
//	}
//}
//
//CXMultiBundle {
//	var <>bundles;
//	
//	add { arg bundle;
//		// check to concat by time
//		bundles = bundles.add(bundle);
//	}
//	send {
//		bundles.do({ arg b; b.send })
//	}
//	asCXBundle {}
//
//}
//
///*
//
//
//
//+ SequenceableCollection {
//	send {
//		// concat to same servers bundles ?
//		
//		// possible timings embedded
//		
//		// msgs as array
//		this.collect({ arg m;
//			m.asArray
//		})
//	
//	}
//	asCXMessage {
//		^CXMessage
//	}
//}
//
//+ Message {
//	send { ^this.value }
//}
//
//
//	setSourceMsg { arg s,atTime;
//		var bundle;
//		bundle = List.new;
//		// do replace, same bus
//		// set patchout of s ?
//		if(source.notNil,{
//			bundle.add( source.stopMsg ); // atTime
//			// deallocate busses !
//			// but keep samples etc.
//			bundle.add( source.freePatchOut );
//		});
//		source = s;
//		bundle.add( source.spawnOnMsg(this.group,this.bus) );
//	}
//
//	.send it
//	
//	Messages in the there too for timing
//	
//*/
//
//
