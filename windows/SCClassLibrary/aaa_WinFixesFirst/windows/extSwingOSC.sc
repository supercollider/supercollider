// fixes 100% CPU problem on Windows for aliveThread

+ SwingOSC {
	startAliveThread { arg delay = 4.0, period = 0.7;
		^aliveThread ?? {
			this.addStatusWatcher;
			aliveThread = Routine({
				// this thread polls the server to see if it is alive
				delay.wait;
				loop {
					this.status;
					period.wait;
					this.serverRunning = alive;
					alive = false;
				};
			});
			AppClock.play(aliveThread);
		}
	}

	boot { arg startAliveThread = true;
		var resp;
		
		Task({
			this.status;
			0.5.wait;
			if( serverRunning, { "SwingOSC server already running".inform; ^this });
			if( serverBooting, { "SwingOSC server already booting".inform; ^this });
			
			serverBooting = true;
			if( startAliveThread, { this.startAliveThread });
//			this.newAllocators;		// will be done in initTree !
//			this.resetBufferAutoInfo;	// not applicable to SwingOSC
			this.doWhenBooted({
// there is no notification system at the moment
//				if( notified, { 
//					this.notify;
//					"notification is on".inform;
//				}, { 
//					"notification is off".inform; 
//				});
				serverBooting = false;
//				serverRunning = true;
//				this.initTree;
			});
			if( isLocal.not, { 
				"You will have to manually boot remote server.".inform;
			},{
				this.bootServerApp;
			});
		}).play(AppClock);
	}
	

	doWhenBooted { arg onComplete, timeout = 20.0;
		var cancel, upd;
		
		if( serverRunning.not, {
			upd		= Updater( this, {
				if( serverRunning, {
					cancel.stop;
					upd.remove;
					onComplete.value;
				});
			});
			cancel	= Task ({
				timeout.wait;
				upd.remove;
				"SwingOSC server failed to start".error;
				serverBooting = false;
			}).play(AppClock);
		}, onComplete );
	}


}