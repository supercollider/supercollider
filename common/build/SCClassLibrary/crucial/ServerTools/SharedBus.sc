
SharedBus : Bus {

	var released = false,<>owner;

	*newFrom { arg bus,owner;
		// only the top owner can free the bus
		if(bus.isKindOf(SharedBus).not,{
			^this.new(bus.rate,bus.index,bus.numChannels,bus.server).owner_(owner)
		},{
			^bus
		})
	}
	releaseBus { arg supposedOwner;
		// first one (top one) to claim it is the real owner
		if(owner === supposedOwner,{
			released = true;
			this.free;
		});
	}
	free {
		if(released,{
			super.free;
		});
	}
	realloc {
		released = true;
		super.realloc;
		released = false;
	}
}
