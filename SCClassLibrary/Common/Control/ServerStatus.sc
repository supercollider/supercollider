ServerStatusWatcher {

	var server;

	var <>notified = false, <notify = true;
	var alive = false, <aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;

	var <hasBooted = false, <>serverBooting = false, <unresponsive = false;

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
		this.serverRunning = false;
		hasBooted = false;
		alive = false;
		notified = false;
		serverBooting = false;
		unresponsive = false;

		// server.changed(\serverRunning) should be deferred in dependants!
		//  just in case some don't, defer here to avoid gui updates breaking.
		defer { server.changed(\serverRunning) };
		bootNotifyFirst = true;
	}

	notify_ { |flag = true|
		notify = flag;
		this.sendNotifyRequest(flag);
	}

	// flag true requests notification, false turns it off
	sendNotifyRequest { |flag = true|
		this.prSendNotifyRequest(flag, false);
	}

	doWhenBooted { |onComplete, limit = 100, onFailure|
		var mBootNotifyFirst = bootNotifyFirst, postError = true;
		bootNotifyFirst = false;

		^Routine {
			while {
				server.serverRunning.not
				/*
				// this is not yet implemented.
				or: { serverBooting and: mBootNotifyFirst.not }
				and: { (limit = limit - 1) > 0 }
				and: { server.applicationRunning.not }
				*/

			} {
				0.2.wait;
			};

			if(server.serverRunning.not, {
				if(onFailure.notNil) {
					postError = (onFailure.value(server) == false);
				};
				if(postError) {
					"Server '%' on failed to start. You may need to kill all servers".format(server.name).error;
				};
				serverBooting = false;
				server.changed(\serverRunning);
			}, {
				// make sure the server process finishes all pending tasks from Server.tree before running onComplete
				server.sync;
				onComplete.value;
			});

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
				if(notify and: { notified.not }) { this.prSendNotifyRequest(true,true) };
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

	serverRunning { ^hasBooted and: notified }

	serverRunning_ { | running |
		if(running != server.serverRunning) {
			this.unresponsive = false;

			hasBooted = running;

			if (running.not) {
				hasBooted = running;
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

	// final actions needed to finish booting
	prFinalizeBoot {
		// this needs to be forked so that ServerBoot and ServerTree will definitely run before
		// notified is true.
		fork({
			ServerBoot.run(server);
			server.sync;
			server.initTree;

			this.notified = true;
			server.changed(\serverRunning);
		}, AppClock)
	}

	// This method attempts to recover from a loss of client-server contact,
	// which is a serious emergency in live shows. So it posts a lot of info
	// on the recovered state, and possibly helpful next user actions.

	prHandleLoginWhenAlreadyRegistered { |clientIDFromProcess|
		"% - handling login request though already registered - \n".postf(server);

		// by default, only reset clientID if changed, to leave allocators untouched:
		if (clientIDFromProcess != server.clientID) {
			// make sure we can set the clientID, and set it
			notified = false;
			server.clientID_(clientIDFromProcess);
			"*** This seems to be a login after a crash, or from a new server object,\n"
			"*** so you may want to release currently running synths by hand:".postln;
			"%.defaultGroup.release;\n".postf(server.cs);
			"*** and you may want to redo server boot finalization by hand:".postln;
			"%.statusWatcher.prFinalizeBoot;\n\n".postf(server.cs);
		} {
			// same clientID, so leave all server resources in the state they were in!
			"This seems to be a login after a loss of network contact - \n"
			"- reconnected with the same clientID as before, so probably all is well.\n".postln;
		};

		// ensure that statuswatcher is in the correct state immediately.
		this.notified = true;
		unresponsive = false;
		server.changed(\serverRunning);
	}

	prSendNotifyRequest { |flag = true, addingStatusWatcher|
		var doneOSCFunc, failOSCFunc;

		if(hasBooted.not) { ^this };

		// set up oscfuncs for possible server responses, \done or \failed
		doneOSCFunc = OSCFunc({ |msg|
			var newClientID = msg[2], newMaxLogins = msg[3];
			failOSCFunc.free;

			if(newClientID.notNil) {
				// notify on:
				// on registering scsynth sends back a free clientID and maxLogins
				// this method doesn't fork/wait so we're still in the clear

				// turn notified off (if it was on) to allow setting clientID
				notified = false;
				server.prHandleClientLoginInfoFromServer(newClientID, newMaxLogins);

				// XXX: this is a workaround because using `serverBooting` is not reliable
				// when server is rebooted quickly.
				if(addingStatusWatcher) {
					this.prFinalizeBoot;
				} {
					notified = true;
				}

			} {
				notified = false;
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

}
