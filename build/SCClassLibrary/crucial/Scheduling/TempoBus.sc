
TempoBus   {

	var <>tempo,bus,isReady = false;
	
	*new { arg server,tempo;
		var new;
		server = server ? Server.local;
		tempo = tempo ?? {Tempo.default};
		// return the same one for any combo
		^(Library.at(this,server.name,tempo) ??
			{
				new = super.new.tempo_(tempo).init(server);
				Library.put(this,server.name,tempo,new);
				new
			}
		)
	}
	releaseBus {
		bus.free;
		bus = nil;
		isReady = false;
	}
	asBus {}
	index { ^bus.index }
	prepareToBundle { arg group,bundle;
		// for now setting duplicate setMsg in there
		bundle.add( bus.setMsg(tempo.tempo) );
		// will trap by isReady later when that is fully dependable
	}
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	synthArg { ^this.index }
	rate { ^\control }
	instrArgFromControl { arg control;
		^control
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
		
		/*  client crashed ?
		SimpleController(server).put(\serverRunning,{
			if(server.serverRunning.not,{
				this.releaseBus;
			},{
				bus = Bus.control(server,1);
				bus.set(tempo.tempo);
				isReady = true;
			})
		})
		*/

		if(server.serverRunning,{ 
			isReady = true;
			bus.value = tempo.tempo;
		});
	}
	update { arg changed,changer;
		if(changed === tempo,{
			bus.server.listSendBundle(bus.server.latency, [bus.setMsg(tempo.tempo)]);
			//bus.value = tempo.tempo;	
		})
	}
}

