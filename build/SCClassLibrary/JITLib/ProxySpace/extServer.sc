+Server {
	//create a bus offset at boot
	bootOffset { arg nAudio=0, nControl=0;
		if (serverRunning, { "server already running".inform; ^this });
		if (serverBooting, { "server already booting".inform; ^this });
		audioBusAllocator.alloc(nAudio);
		controlBusAllocator.alloc(nControl);
		this.boot;
	}

}