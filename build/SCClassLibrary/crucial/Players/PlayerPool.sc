PlayerPool : PlayerSocket { 	// implements selectable interface	var <>selected, <>list,<>autostart=false;	var socket;
		*new { arg list,selected=0,env,round=0.0;		^super.new(round)			.list_(loadDocument(list))			.selected_(selected)			.env_(env ?? {Env.new([ 0, 1.0, 0 ], [ 0.01, 0.7 ], -4, 1, nil)})	}	storeArgs { ^[list, selected ,env,round ]  }
	addPlayer { arg player;
		var path;
		player = loadDocument(player);
		list = list.add(player);
		this.changed(\players);	
	}

	//selectable interface	select { arg i;		selected = i;		this.changed; // sched for time ?		if(this.isPlaying,{
			this.qtrigger(list.at(i));
		})
	}	selectedItem { ^list.at(selected) }		selectedAsString {		^list.at(selected).asString	}	maxIndex { ^list.size }			choose {		this.select(list.size.rand)	}	rate { ^list.first.rate } // what else to do ?	numChannels { ^list.maxValue({ arg pl; pl.numChannels }) }	releaseAll { socket.release }	children { ^list }	
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		list.do({ arg pl; pl.prepareToBundle(group,bundle) });
		super.prepareToBundle(group,bundle);
	}
	loadDefFileToBundle { arg bundle,server;
		list.do({ arg pl;
			pl.loadDefFileToBundle(bundle,server)
		});
	}

	guiClass { ^PlayerPoolGui }}/*PatchSwitcher : PlayerPool {

	var <>inputs,ip,proxyMatches;
	var connectedInputs,inputGroup;
	
	*new { arg patches,inputs,selected=0,env,round=0.0;
		^super.new(patches,selected,env,round).inputs_(loadDocument(inputs)).psinit
	}
	psinit {
		ip = list.collect({ arg p;	p.annotatedInputProxies  });
		connectedInputs = Array.newClear(inputs.size);
		proxyMatches = Array.newClear(list.size);
		
		ip.do({ arg annInProxyLists,pli;	
			annInProxyLists.do({ arg annInProxyList;
				var inProxy,ai,argName,spec;
				# inProxy, ai, argName, spec = annInProxyList;
				inputs.do({ arg input,inpi;
					
					if(input.key.isKindOf(Symbol),{
						if(argName === input.key,{
							// match
							proxyMatches.put(pli, proxyMatches.at(pli).add( [inProxy , input.value ] ));
						})
					},{
						if(input.key.isKindOf(Spec),{
							if(input.key == spec,{
								proxyMatches.put(pli, proxyMatches.at(pli).add( [inProxy , input.value ]));
							})
						})
					});
					// also support list and a dual output player / editor
				});
			})
		})
	}
	select { arg i;
		var player,bundle,atTime,playing;
		selected = i;
		this.changed; // sched for time ?
		if(this.isPlaying,{
			// sched this
			//connectedInputs.do({ arg ci;
			//	if(ci.notNil,{ ci.disconnect });
			//});
			player = list.at(i);

			// set the initial value (maybe slightly wrong by qtrigger time)
			bundle = List.new;
			proxyMatches.at(i).do({ arg inpinp;
				var inputProxy,input;
				# inputProxy , input = inpinp;
				if((input.isPlaying ? true).not,{
					input.spawnOnToBundle(inputGroup,bundle: bundle);
				});
				inputProxy.initValue = input.synthArg;
			});
			bundle.add( this.setSourceMsg(player) );
			atTime = this.qtime;
			this.server.listSendBundle( atTime, bundle);
			// much better to have the connectTo actions as messages
			SystemClock.sched(atTime,{
				// on start, connectTo, set value
				// matches connectTo(inProxy)
				"connecting".postln;
				proxyMatches.at(i).do({ arg inpinp;
					var inputProxy,input;
					# inputProxy , input = inpinp;
					input.connectTo( inputProxy )
				});
			});
		})
	}
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		// make an input group
		inputGroup = Group.basicNew;
		// will always stay ahead
		bundle.add( inputGroup.addToHead(group) );
		
		inputs.do({ arg inp; inp.prepareToBundle(group,bundle) });
		super.prepareToBundle(group,bundle);
	}
	spawnAtTime { arg atTime; // undo super
		var bundle;
		bundle = List.new;
		this.spawnToBundle(bundle);
		this.sendSpawnBundle(bundle,atTime);
		if(atTime.isNumber,{
			AppClock.sched(atTime,{ isPlaying = true; nil });
		},{
			isPlaying = true;
		})
	}
//	spawnToBundle { arg bundle;
//		inputs.do({ arg inp; inp.value.spawnToBundle(bundle) });
//		super.spawnToBundle(bundle);
//	}
//	didSpawn { patchIn, synthArgi;
//		super.didSpawn(patchIn, synthArgi);
//		inputs.do({ arg inp,inpi; 
//			// playing initially ?
//			// use select
//			inp.value.didSpawn
//		})
//	}
}

*/
