
SharedBus : Bus {

	var released = false;
	
	*newFrom { arg bus;
		^this.new(bus.rate,bus.index,bus.numChannels,bus.server)
	}
	releaseBus {
		released = true;
		this.free;
	}
	free {
		if(released,{
			super.free
		})
	}

}