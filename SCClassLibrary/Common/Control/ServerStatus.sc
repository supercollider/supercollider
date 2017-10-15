ServerStatusWatcher {

	var server;

	var <>notified = false, <notify = true;
	var alive = false, <aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;

	var <serverRunning = false, <>serverBooting = false, <unresponsive = false;

	var <numUGens=0, <numSynths=0, <numGroups=0, <numSynthDefs=0;
	var <avgCPU, <peakCPU;
	var <sampleRate, <actualSampleRate;

	var reallyDeadCount = 0, bootNotifyFirst = true;

	*new { |server|
		^super.newCopyArgs(server)
	}

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

	sendNotifyRequest { |flag = true|
		var doneOSCFunc, failOSCFunc;
		var desiredClientID;

		if(serverRunning.not) { ^this };

		// flag true requests notification, false turns it off
		notified = flag;

		// set up oscfuncs for possible server responses, \done or \failed
		doneOSCFunc = OSCFunc({|msg|
			var newClientID = msg[2], newMaxLogins = msg[3];
			failOSCFunc.free;

			if(flag) {
				// on registering scsynth sends back a free clientID and its maxLogins,
				// which usually adjust the server object's settings:
				this.prHandleClientLoginInfoFromServer(newClientID, newMaxLogins);
			};

		}, '/done', server.addr, argTemplate:['/notify', nil]).oneShot;

		failOSCFunc = OSCFunc({|msg|

			doneOSCFunc.free;
			this.prHandleNotifyFailString(msg[2], msg);

		}, '/fail', server.addr, argTemplate:['/notify', nil, nil]).oneShot;

		// send the notify request - on or off, userSpecifiedClientID or not
		desiredClientID = if(server.userSpecifiedClientID, { server.clientID }, {-1});
		server.sendMsg("/notify", flag.binaryValue, desiredClientID);

		if(flag){
			"Requested notification messages from server '%'\n".postf(server.name)
		} {
			"Switched off notification messages from server '%'\n".postf(server.name);
		};
	}

	prHandleClientLoginInfoFromServer { |newClientID, newMaxLogins|
		if (newMaxLogins.notNil) {
			if (newMaxLogins != server.options.maxLogins) {
				"%: scsynth has maxLogins % - adjusting my options accordingly.\n"
				.postf(server, newMaxLogins);
				server.options.maxLogins = newMaxLogins;
			} {
				"%: scsynth maxLogins % match with my options.\n".postf(server, newMaxLogins);
			};
		} {
			"%: no maxLogins info from scsynth.\n".postf(server, newMaxLogins);
		};

		if (newClientID == server.clientID) {
			"%: keeping clientID % as confirmed from scsynth.\n"
			.postf(server, newClientID);
		} {
			if (server.userSpecifiedClientID.not) {
				"%: setting clientID to %, as obtained from scsynth.\n"
				.postf(server, newClientID);
			} {
				("% - userSpecifiedClientID % is not free!\n"
					" Switching to free clientID obtained from scsynth: %.\n"
					"If that is problematic, please set clientID by hand before booting.")
				.format(server, server.clientID, newClientID).warn;
			};
		};
		server.clientID = newClientID;
	}

	prHandleNotifyFailString {|failString, msg|

		// post info on some known error cases
		case
		{ failString.asString.contains("already registered") } {
			"% - already registered with clientID %.\n".postf(server, msg[3])
		} { failString.asString.contains("not registered") } {
			// unregister when already not registered:
			"% - not registered.\n".postf(server)
		} { failString.asString.contains("too many users") } {
			"% - could not register, too many users.\n".postf(server)
		} {
			// throw error if unknown failure
			Error(
				"Failed to register with server '%' for notifications: %\n"
				"To recover, please reboot the server.".format(server.name, msg)).throw;
		};
	}

	doWhenBooted { |onComplete, limit = 100, onFailure|
		var mBootNotifyFirst = bootNotifyFirst, postError = true;
		bootNotifyFirst = false;

		^Routine {
			while {
				serverRunning.not
				/*
				// this is not yet implemented.
				or: { serverBooting and: mBootNotifyFirst.not }
				and: { (limit = limit - 1) > 0 }
				and: { server.applicationRunning.not }
				*/

			} {
				0.2.wait;
			};
			if(serverRunning.not, {
				if(onFailure.notNil) {
					postError = (onFailure.value(server) == false);
				};
				if(postError) {
					"Server '%' on failed to start. You may need to kill all servers".format(server.name).error;
				};
				serverBooting = false;
				server.changed(\serverRunning);
			}, onComplete);

		}.play(AppClock)
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

	stopStatusWatcher {
		statusWatcher !? { statusWatcher.disable }
	}

	startAliveThread { | delay = 0.0 |
		this.addStatusWatcher;
		^aliveThread ?? {
			aliveThread = Routine {
				// this thread polls the server to see if it is alive
				delay.wait;
				loop {
					alive = false;
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

	serverRunning_ { | running |

		if(running != serverRunning) {
			serverRunning = running;
			this.unresponsive = false;

			if (server.serverRunning) {
				ServerBoot.run(server);
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
			{ server.changed(\serverRunning) }.defer;
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
