
PlayerPool : PlayerSocket { 	// implements selectable interface

	var <>selected, <>list, <>autostart=false;
	
	*new { arg list,selected=0,env,round=0.0;
		^super.new(round)
			.list_(loadDocument(list))
			.selected_(selected)
			.env_(env ?? {Env.new([ 0, 1.0, 0 ], [ 0.01, 0.7 ], -4, 1, nil)})
	}
	storeArgs { ^[list, selected ,env,round ]  }
	addPlayer { arg player;
		var path;
		player = loadDocument(player);
		list = list.add(player);
		if(this.isPlaying,{
			this.preparePlayer(player);
		});
		this.changed(\players);	
	}

	//selectable interface
	select { arg i;
		selected = i;
		if(this.isPlaying,{
			this.qspawnPlayer(list.at(i));
		},{
			source = lastPlayer = list.at(i);
		})
	}
	selectedItem { ^list.at(selected) }
	
	selectedAsString {
		^list.at(selected).asString
	}
	maxIndex { ^list.size }
		
	choose {
		this.select(list.size.rand)
	}
	rate { ^list.first.rate } // what else to do ?
	numChannels { ^list.maxValue({ arg pl; pl.numChannels }) }
	children { ^list }
	// PlayerSocket doesn't prepare children
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		list.do({ arg pl; pl.prepareToBundle(group,bundle) });
		super.prepareToBundle(group,bundle);
	}
	spawnToBundle { arg bundle;
		if(autostart,{
			this.setSourceToBundle(this.selectedItem,bundle);
		},{
			if(source.notNil,{		
				source.spawnToBundle(bundle)
			})
		});
		bundle.addAction(this,\didSpawn);
	}

	guiClass { ^PlayerPoolGui }
}

/**
PatchSwitcher : PlayerPool {

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
							proxyMatches.put(pli, proxyMatches.at(pli).add( [inProxy , input.value, argName ] ));
						})
					},{
						if(input.key.isKindOf(Spec),{
							if(input.key == spec,{
								proxyMatches.put(pli, proxyMatches.at(pli).add( [inProxy , input.value, argName ]));
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

			// set the initial value (maybe slightly wrong by qspawnPlayer time)
			bundle = CXBundle.new;
			proxyMatches.at(i).do({ arg inpinp;
				var inputProxy,input;
				# inputProxy , input = inpinp;
				if((input.isPlaying ? false).not,{
					input.spawnOnToBundle(inputGroup,bundle: bundle).insp("input spawnd to bun");
				});
				inputProxy.initValue = input.synthArg;
			});
			this.setSourceToBundle(player,bundle);
			bundle.addFunction({
				// on start, connectTo, set value
				// matches connectTo(inProxy)
				proxyMatches.at(i).do({ arg inpinp;
					var inputProxy,input,argName;
					# inputProxy , input, argName = inpinp;
					// haven't yet set the nodeControl of patchIn of inputProxy
					inputProxy.setNodeControl( NodeControl(input.synth,argName).insp("nodecon") );
					input.connectTo( inputProxy )
				});
			});

			atTime = BeatSched.tdeltaTillNext(round);
			bundle.send(server,atTime);
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
		bundle = CXBundle.new;
		this.spawnToBundle(bundle);
		bundle.addFunction({ isPlaying = true });
		bundle.send(this.server,atTime);
	}
}

**/

