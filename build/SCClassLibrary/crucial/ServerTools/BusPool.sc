
/*
	in the style of Objective-C retain/release memory management.
	this is useful for situations where many clients may use the same
	bus, handing it to each other.  each one retains, each one releases.
	when the bus is released for the last time it is freed
*/

BusPool {

	classvar <counts,<annotations;

	*alloc { |rate=\audio,server,numChannels=1,client,name|
		var prev,bus;
		bus = Bus.alloc(rate,server,numChannels);
		this.retain(bus,client,name);
		^bus
	}
	*retain { |bus,client,name|
		counts.add(bus);
		if(annotations.at(bus,client).notNil,{
			Error("A client may only retain a bus once").throw;
		});
		//[bus,client,name].debug("retain");
		annotations.put(bus,client,name);
		this.watchServer(bus.server);
	}
	*release { |bus,client|
		var dict,key;
		counts.remove(bus);
		annotations.removeAt(bus,client);
		if(counts.itemCount(bus) == 0,{
			//bus.debug("count 0, freeing bus");
			bus.free;
		},{
			//bus.debug("count > 0, hanging on"+counts.itemCount(bus));
		})
	}

	*initClass {
		this.reset;
	}
	*reset {
		if(counts.notNil,{
			counts.contents.keysValuesDo({ |bus,count| bus.free });
		});
		counts = Bag.new;
		annotations = MultiLevelIdentityDictionary.new;
	}
	*watchServer { |server|
		if(NotificationCenter.registrationExists(server,\newAllocators,this).not,{
			NotificationCenter.register(server,\newAllocators,this,{
				//"new allocators, BusPool".debug;
				this.reset;
			});
		});
	}
	*itemCount { |bus| ^counts.itemCount(bus) }
	*busses { ^counts.contents.keys.as(Array) }
	*gui {
		Sheet({ |f|
			counts.contents.keysValuesDo({ |bus,count|
				f.startRow;
				bus.gui(f);
				count.gui(f);
				f.flow({ |f|
					annotations[bus].keysValuesDo({ |client,name|
						f.startRow;
						Tile(client,f);
						CXLabel(f,":"++name);
					});
				})
			});
			if(counts.contents.size == 0,{
				CXLabel(f,"No Busses allocated in BusPool");
			});
		})
	}
	*getAnnotations { |bus|
		^annotations[bus]
	}
	*findBus { |server,index|
		^counts.detect({ |bus|
			bus.index == index and: {bus.server === server}
		})
	}
	*makeBusFromSpec { |busSpec,server,client,name|
		var bus;
		if(busSpec.rate == \audio,{
			if(busSpec.private,{
				bus = this.alloc(\audio,server,busSpec.numChannels,client,name)
			},{
				bus = Bus.new(\audio,0,busSpec.numChannels,server);
				this.retain(bus,client,name)
			})
		},{
			bus = this.alloc(\control,server,busSpec.numChannels,client,name)
		});
		^bus
	}


}

