ServerStatusWatcher {

	classvar bootStatuses = #[\off, \booting, \doingSetup, \running, \quitting];

	var server;

	var <>notified = false, <notify = true;
	var alive = false, <aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;

	var <bootStatus = \off;
	var <hasBooted = false, <>serverBooting = false, <unresponsive = false;

	var <numUGens=0, <numSynths=0, <numGroups=0, <numSynthDefs=0;
	var <avgCPU, <peakCPU;
	var <sampleRate, <actualSampleRate;

	var reallyDeadCount = 0, bootNotifyFirst = true;

	*new { |server|
		^super.newCopyArgs(server)
	}

	bootStatus_ { |newStatus|
		if (bootStatuses.includes(newStatus)) {
			bootStatus = newStatus;
			"%: bootStatus is now %.\n".postf(server, newStatus.cs);
		} {
			warn(
				"%:% - % is not a valid status."
				.format(this, thisMethod.name, newStatus)
			);
		}
	}
	running { ^bootStatus == \running }
	doingSetup { ^bootStatus == \doingSetup }
	booting { ^bootStatus == \running }
	quitting { ^bootStatus == \quitting }

	quit { |onComplete, onFailure, watchShutDown = true|
		if(watchShutDown) {
			this.watchQuit(onComplete, onFailure)
		} {
			this.stopStatusWatcher;
			onComplete.value;
		};
		this.stopAliveThread;
		notified = false;
		serverBooting = false;
		this.serverRunning = false;
		bootNotifyFirst = true;
	}

	notify_ { |flag = true|
		notify = flag;
		this.sendNotifyRequest(flag);
	}

	// flag true requests notification, false turns it off
	sendNotifyRequest { |flag = true|
		var doneOSCFunc, failOSCFunc;

		if(hasBooted.not) { ^this };

		// set up oscfuncs for possible server responses, \done or \failed
		doneOSCFunc = OSCFunc({ |msg|
			var newClientID = msg[2], newMaxLogins = msg[3];
			failOSCFunc.free;

			if (newClientID.isNil) {
				// notify off got done response:
				notified = false;
			} {
				// notify on:
				// on registering scsynth sends back a free clientID and its maxLogins,
				// which usually adjust the server object's settings:
				server.prHandleClientLoginInfoFromServer(newClientID, newMaxLogins);
			};

		}, '/done', server.addr, argTemplate:['/notify', nil]).oneShot;

		failOSCFunc = OSCFunc({|msg|

			doneOSCFunc.free;
			server.prHandleNotifyFailString(msg[2], msg);

		}, '/fail', server.addr, argTemplate:['/notify', nil, nil]).oneShot;

		server.sendMsg("/notify", flag.binaryValue, server.clientID);

		if(flag){
			"Requested notification messages from server '%'\n".postf(server.name)
		} {
			"Switched off notification messages from server '%'\n".postf(server.name);
		};
	}

	doWhenBooted { |onComplete, limit = 100, onFailure|
		this.deprecated(thisMethod, Server.findMethod(\doWhenBooted));
		server.doWhenBooted(onComplete, limit = 100, onFailure);
	}


	watchQuit { |onComplete, onFailure|
		var	serverReallyQuitWatcher, serverReallyQuit = false;
		statusWatcher !? {
			statusWatcher.disable;
			if(notified) {
				serverReallyQuitWatcher = OSCFunc({ |msg|
					if(msg[1] == '/quit') {
						statusWatcher !? { statusWatcher.enable };
						serverReallyQuit = true;
						serverReallyQuitWatcher.free;
						onComplete.value;
					}
				}, '/done', server.addr);

				AppClock.sched(3.0, {
					if(serverReallyQuit.not) {
						if(unresponsive) {
							"Server '%' remained unresponsive during quit."
						} {
							"Server '%' failed to quit after 3.0 seconds."
						}.format(server.name).warn;
						// don't accumulate quit-watchers if /done doesn't come back
						serverReallyQuitWatcher.free;
						statusWatcher !? { statusWatcher.disable };
						onFailure.value(server)
					}
				})
			}
		}
	}


	addStatusWatcher {
		if(statusWatcher.isNil) {
			statusWatcher =
			OSCFunc({ arg msg;
				var cmd, one;
				if(notify and: { notified.not }) { this.sendNotifyRequest };
				alive = true;
				#cmd, one, numUGens, numSynths, numGroups, numSynthDefs,
				avgCPU, peakCPU, sampleRate, actualSampleRate = msg;
				{
					this.updateRunningState(true);
					server.changed(\counts);
				}.defer;
			}, '/status.reply', server.addr).fix;
		} {
			statusWatcher.enable;
		}
	}

	stop {
		statusWatcher !? { statusWatcher.disable }
	}

	stopStatusWatcher { this.stop }
	startAliveThread { this.start }

	start { | delay = 0.0 |
		if (Server.postingBootInfo) {
			"%.% with delay %.\n".postf(server, thisMethod.name, delay);
		};
		this.addStatusWatcher;
		^aliveThread ?? {
			aliveThread = Routine {
				// this thread polls the server to see if it is alive
				delay.wait;
				loop {
					alive = false;
					if (Server.postingBootInfo) {
						"% . sendStatusMsg...\n".postf(server);
					};
					server.sendStatusMsg;
					aliveThreadPeriod.wait;
					this.updateRunningState(alive);
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

	serverRunning { ^hasBooted and: notified }

	serverRunning_ { | running |

		hasBooted = running;
		if(running != server.serverRunning) {
			hasBooted = running;

			this.unresponsive = false;

				// serverBoot happens in prRunBootTask now
			if (running) {
				server.bootStatus_(\doingSetup)
			} {
				ServerQuit.run(server);

				server.disconnectSharedMemory;
				if(server.isRecording) { server.stopRecording };

				{ server.changed(\didQuit) }.defer;
				NotificationCenter.notify(server, \didQuit); // why here "NotificationCenter" and below "changed"?

				if(server.isLocal.not) {
					notified = false;
				};
			};
		}

	}

	updateRunningState { | running |
		if(server.addr.hasBundle) {
			{ server.changed(\bundling) }.defer;
		} {
			if(running) {
				this.serverRunning = true;
				this.unresponsive = false;
				reallyDeadCount = server.options.pingsBeforeConsideredDead;
			} {
				// parrot
				reallyDeadCount = reallyDeadCount - 1;
				this.unresponsive = (reallyDeadCount <= 0);
			}
		}
	}

	unresponsive_ { | val |
		if (val != unresponsive) {
			unresponsive = val;
			{ server.changed(\serverRunning) }.defer;
		}
	}

}
