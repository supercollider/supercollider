
PlayerPool : PlayerSocket { 	// implements selectable interface

	var <>selected, <>list, <>autostart=false;
	
	*new { arg list,selected=0,env,round=0.0;
		^super.new
			.list_(loadDocument(list))
			.selected_(selected)
			.env_(env ?? {Env.new([ 0, 1.0, 0 ], [ 0.01, 0.7 ], -4, 1, nil)})
			.round_(round)
			//.select(selected)
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

	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		// the superclass doesn't prepare the children
		list.do({ arg pl; pl.prepareToBundle(socketGroup,bundle) });
		super.prepareToBundle(group,bundle);
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
