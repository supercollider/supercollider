
TempoBus   {

	var <>tempo,bus,isReady = false;
	
	*new { arg server,tempo;
		var new;
		server = server ? Server.local;
		tempo = tempo ? Tempo.default;
		// return the same one for any combo
		^(Library.at(this,server.name,tempo) ??
			{
				new = super.new.tempo_(tempo).init(server);
				Library.put(this,server.name,tempo,new);
				new
			}
		)
	}
	asBus {}
	index { ^bus.index }
	prepareToBundle { arg group,bundle;
		// ignores the group
		//if(isReady.not,{ // just in case server wasn't booted before on init
			bundle.add( bus.setMsg(tempo.tempo) );
			//isReady = true;
		//})
	}
	
	free {
		// for now just leave it on the server, its cheap.
		// we need reference counting, since its a shared object.
		
		//bus.free;
		//bus = nil;
		//isReady = false;
	}
	
	init { arg server;
		bus = Bus.control(server,1);
		bus.set(tempo.tempo);
		tempo.addDependant(this);
		
		// could depend on the server too (serverRunning)
		
		if(server.serverRunning,{ 
			isReady = true;
			bus.value = tempo.tempo;
		});
	}
	update { arg changed,changer;
		if(changed === tempo,{
			bus.value = tempo.tempo;	
		})
	}

}

