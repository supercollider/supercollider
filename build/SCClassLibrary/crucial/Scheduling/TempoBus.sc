
TempoBus  {

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
	index { ^bus.index }	
	
	free {
		bus.free;
		bus = nil;
		// who will call this ?  when ?
		// never maybe, not worth it.
		isReady = false;
	}
	
	init { arg server;
		bus = Bus.control(server,1);
		tempo.addDependant(this);
		
		// is it already ready ?
		isReady = true;
		bus.value = tempo.tempo;
	}
	update { arg changed,changer;
		//[changed,changer,tempo].postln;
		if(changed === tempo,{
			bus.value = tempo.tempo;	
		})
	}

}

