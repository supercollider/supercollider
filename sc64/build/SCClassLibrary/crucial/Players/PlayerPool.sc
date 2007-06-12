
PlayerPool : PlayerSocket { 	// implements selectable interface

	var <>selected, <>list, <>autostart=false;
	
	*new { arg list,selected=0,env,round=0.0;
		list = loadDocument(list);
		^super.prNew(list.first.rate,
				//with patches they still don't know
				list.maxValue({ arg it; it.numChannels }),
				round, 
				env ?? {Env.new([ 0, 1.0, 0 ], [ 0.01, 0.7 ], -4, 1, nil)})
			.list_(list)
			.select(selected)
	}
	storeArgs { ^[list, selected ,env,round ]  }
	addPlayer { arg player;
		var path;
		player = loadDocument(player);
		list = list.add(player);
		numChannels = max(numChannels,player.numChannels);
		if(this.isPlaying,{
			this.preparePlayer(player);
		});
		this.changed(\players);	
	}

	select { arg i;
		selected = i;
		if(this.isPlaying,{
			// were all prepared
			this.prepareAndQSpawn(list.at(i));
			// but after a release they lose their patch out
			//this.qspawnPlayer(list.at(i));
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
	children { ^list }

	prepareChildrenToBundle { arg bundle;
		list.do({ arg child;
			child.prepareToBundle(socketGroup,bundle,true,sharedBus,true);
		});
	}
	spawnToBundle { arg bundle;
		if(autostart,{
			if(round == 0.0,{
				this.setSourceToBundle(this.selectedItem,bundle);
			},{
				bundle.addFunction({
					// xblock on these too !
					this.qspawnPlayer(list.at(selected));
				})
			});
		},{
			if(source.notNil,{
				// if you select while play is starting
				// the source is then set, which then gets started here
				// but this may not be on the 'round'
				bundle.addFunction({
					// xblock on these too !
					this.qspawnPlayer(list.at(selected));
				})
			})
		});
		bundle.addMessage(this,\didSpawn);
	}

	guiClass { ^PlayerPoolGui }
}
