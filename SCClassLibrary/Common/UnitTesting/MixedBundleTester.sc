
/*
	saves all sent messages so that UnitTests can query it later

	but its not a Mock object : it still sends to the server

	could be implemented as one though

*/

MixedBundleTester : MixedBundle {

	classvar <bundlesSent;

	// private //
	prSend { arg server, latency;
		latency = latency ?? { server.latency };
		super.prSend(server,latency);
		SystemClock.sched(latency,{
			bundlesSent = bundlesSent.add( this );
		})
	}
	clumpedSendNow { arg server;
		if(messages.notNil,{
			messages.clump(10).do({ arg bundle,i;
				server.listSendBundle(i * 0.001,bundle);
			});
			SystemClock.sched(messages.size * 0.001,{
				this.doFunctions;
				bundlesSent = bundlesSent.add(this);
				nil
			});
			^messages.size
		},{
			this.doFunctions;
			bundlesSent = bundlesSent.add(this);
			^0
		})
	}
	*reset {
		bundlesSent = [];
	}
	// matches message :
	// [9,"defName"]  matches any [9,"defName" (, 1001,0,1)]
	*findMessage { arg message;
		if(bundlesSent.isNil,{ ^false });
		^bundlesSent.any({ |b|
			if(b.messages.isNil,{
				false
			},{
				b.messages.any({ |m|
					if(m.size < message.size,{
						false
					},{
						m.copyRange(0,message.size - 1) == message
					})
				})
			})
		})
	}
	// matches message :
	// ["/d_recv"] matches any [/d_recv, (data )]
	*findPreparationMessage { arg message;
		if(bundlesSent.isNil,{ ^false });
		^bundlesSent.any({ |b|
			if(b.preparationMessages.isNil,{
				false
			},{
				b.preparationMessages.any({ |pm|
					if(pm.size < message.size,{
						false
					},{
						pm.copyRange(0,message.size - 1) == message
					})
				})
			})
		})
	}
	
	*report {
		bundlesSent.do({ |bnd,i|
			Post << "######## Bundle " << i << " ###########################" << Char.nl;
			"preparationMessages:".postln;
			bnd.preparationMessages.do({ |pm|
				Post << Char.tab << pm << Char.nl;
			});
			"messages:".postln;
			bnd.messages.do({ |msg|
				Post << Char.tab << msg << Char.nl;
			});
			"sendFunctions:".postln;
			bnd.sendFunctions.do({ |sf|
				Post << Char.tab << sf << Char.nl;
			});
			"functions:".postln;
			bnd.functions.do({ |sf|
				Post << Char.tab;
				if(sf.isKindOf(Function),{
					sf.instVarAt('context').postln;
				},{
					if(sf.isKindOf(Message),{
						"Message %:%(%)".format(sf.receiver,sf.selector, sf.args).postln;
					},{
						sf.postln;
					});
				});
			});
			"".postln;
		})
	}
	// parse preparation messages for the def names of d_recv
	defNames {
		var names;
		if(preparationMessages.isNil,{ ^[] });
		names = preparationMessages.select({ |msg| msg[0] == "/d_recv" })
					.collect({ |msg| SynthDesc.defNameFromBytes(msg[1]) });
		^names
	}
	includesDefName { arg defName;
		if(defName.isNil,{ Error("MixedBundleTester-includesDefName : defName was nil").throw });
		^this.defNames.indexOfEqual(defName).notNil
	}
 
}

