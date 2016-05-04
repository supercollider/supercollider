ServerStatusWatcher {

	var server;

	var <>notified = false, <notify = false;
	var alive = false, <aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;

	var <serverRunning = false, <>serverBooting = false, <unresponsive = false;

	var <numUGens=0, <numSynths=0, <numGroups=0, <numSynthDefs=0;
	var <avgCPU, <peakCPU;
	var <sampleRate, <actualSampleRate;

	var reallyDeadCount = 0;
	var <>bootNotifyFirst;


	*new { |server|
		^super.newCopyArgs(server)
	}

	quit { |watchShutDown = true|
		if(watchShutDown) {
			this.watchQuit
		} {
			this.stopStatusWatcher
		};
		this.stopAliveThread;
		notified = false;
		serverBooting = false;
		this.serverRunning = false;
	}

	notify_ { |flag = true|
		notify = flag;
		if(flag){
			if(serverRunning){
				this.sendNotifyRequest(true);
				notified = true;
				"Receiving notification messages from server %\n".postf(server.name);
			}
		}{
			this.sendNotifyRequest(false);
			notified = false;
			"Switched off notification messages from server %\n".postf(server.name);
		}
	}

	sendNotifyRequest { | flag=true |
		var doneOSCFunc, failOSCFunc;
		notified = flag;
		if(server.userSpecifiedClientID.not) {
			doneOSCFunc = OSCFunc({|msg|
				if(flag) { server.clientID = msg[2] };
				failOSCFunc.free;
			}, '/done', server.addr, argTemplate:['/notify', nil]).oneShot;

			failOSCFunc = OSCFunc({|msg|
				server.clientID = msg[2];
				doneOSCFunc.free;
			}, '/fail', server.addr, argTemplate:['/notify', nil, nil]).oneShot;

		};

		server.sendMsg("/notify", flag.binaryValue);
	}

	doWhenBooted { arg onComplete, limit=100, onFailure;
		var mBootNotifyFirst = bootNotifyFirst, postError = true;
		bootNotifyFirst = false;

		^Routine {
			while {
				((serverRunning.not
					or: (serverBooting and: mBootNotifyFirst.not))
				and: { (limit = limit - 1) > 0 })
				and: { server.pid.tryPerform(\pidRunning) == true }
			} {
				0.2.wait;
			};

			if(serverRunning.not, {
				if(onFailure.notNil) {
					postError = (onFailure.value == false);
				};
				if(postError) {
					"server failed to start".error;
					"For advice: [http://supercollider.sf.net/wiki/index.php/ERROR:_server_failed_to_start]".postln;
				};
				serverBooting = false;
				server.changed(\serverRunning);
			}, onComplete);
		}.play(AppClock);
	}


	watchQuit {
		var	serverReallyQuitWatcher, serverReallyQuit = false;
		statusWatcher !? {
			statusWatcher.disable;
			if(notified) {
				serverReallyQuitWatcher = OSCFunc({ |msg|
					if(msg[1] == '/quit') {
						if (statusWatcher.notNil) {
							statusWatcher.enable;
						};
						serverReallyQuit = true;
						serverReallyQuitWatcher.free;
					};
				}, '/done', server.addr);

				AppClock.sched(3.0, {
					if(serverReallyQuit.not) {
						"Server % failed to quit after 3.0 seconds.".format(server.name).warn;
						// don't accumulate quit-watchers if /done doesn't come back
						serverReallyQuitWatcher.free;
						statusWatcher.disable;
					};
				});
			};
		};
	}


	addStatusWatcher {
		if(statusWatcher.isNil) {
			statusWatcher =
			OSCFunc({ arg msg;
				var cmd, one;
				if(notify){
					if(notified.not){
						this.sendNotifyRequest;
						"Receiving notification messages from server %\n".postf(server.name);
					}
				};
				alive = true;
				#cmd, one, numUGens, numSynths, numGroups, numSynthDefs,
						avgCPU, peakCPU, sampleRate, actualSampleRate = msg;
				{
					this.updateRunningState(true);
					this.changed(\counts);
					nil // no resched
				}.defer;
			}, '/status.reply', server.addr).fix;
		} {
			statusWatcher.enable;
		}
	}

	stopStatusWatcher {
		statusWatcher.disable;
	}

	startAliveThread { | delay=0.0 |
		this.addStatusWatcher;
		^aliveThread ?? {
			aliveThread = Routine {
				// this thread polls the server to see if it is alive
				delay.wait;
				loop {
					server.status;
					aliveThreadPeriod.wait;
					this.updateRunningState(alive);
					alive = false;
				};
			}.play(AppClock);
			aliveThread
		}
	}

	stopAliveThread {

		statusWatcher.free;
		statusWatcher = nil;

		aliveThread.stop;
		alive = false;
		aliveThread = nil;
	}

	resumeThread {
		aliveThread !? {
			this.stopAliveThread;
			this.startAliveThread;
		}
	}


	serverRunning_ { | val |

		if(val != serverRunning) {
			serverRunning = val;
			unresponsive = false;

			if (server.serverRunning) {
				ServerBoot.run(server);
			} {
				ServerQuit.run(server);

				server.disconnectSharedMemory;
				if(server.isRecording) { server.stopRecording };

				NotificationCenter.notify(server, \didQuit);

				if(server.isLocal.not) {
					notified = false;
				};
			};
			{ server.changed(\serverRunning) }.defer;
		}

	}

	updateRunningState { | val |
		if(server.addr.hasBundle) {
			{ server.changed(\bundling) }.defer;
		} {
			if(val) {
				this.serverRunning = true;
				unresponsive = false;
				reallyDeadCount = server.options.pingsBeforeConsideredDead;
			} {
				reallyDeadCount = reallyDeadCount - 1;
				this.unresponsive = (reallyDeadCount <= 0);
			}
		}
	}


	unresponsive_ { arg val;
		if (val != unresponsive) {
			unresponsive = val;
			{ server.changed(\serverRunning) }.defer;
		}
	}

}