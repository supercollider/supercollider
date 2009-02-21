

AbstractSystemAction {

	*init {
		this.objects = List.new;
	}
	
	*add { arg object;
		if(this.objects.isNil) { this.init }; // lazy init
		if(this.objects.includes(object).not) { this.objects.add(object) }
	}
	
	*remove { arg object;
		this.objects.remove(object)
	}
	
	*removeAll {
		this.init
	}

	*objects { ^this.shouldNotImplement(thisMethod) }
	*objects_ { arg obj; ^this.shouldNotImplement(thisMethod) }
}



// things to clear when hitting cmd-.

CmdPeriod : AbstractSystemAction {
	classvar <>objects;
	classvar <era = 0;
	classvar <>clearClocks = true;
	classvar <>freeServers = true;
	classvar <>freeRemote = false;



	*doOnce { arg object;
		var f = { this.remove(f); object.doOnCmdPeriod  };
		this.add(f);
	}
	*run {
		if(clearClocks, {
			SystemClock.clear;
			AppClock.clear;
	//		TempoClock.default.clear;
		});
	
		objects.do({ arg item; item.doOnCmdPeriod;  });
	
		if(freeServers, {
			Server.freeAll(freeRemote); // stop all sounds on local, or remote servers
			Server.resumeThreads;
		});
	
		era = era + 1;

	}

	*hardRun {

		SystemClock.clear;
		AppClock.clear;
		TempoClock.default.clear;

		objects.do({ arg item; item.doOnCmdPeriod;  });

		

		Server.hardFreeAll; // stop all sounds on local servers
		Server.resumeThreads;
		era = era + 1;

	}


}


// things to do after startup file executed

StartUp : AbstractSystemAction {


	classvar <>objects, <done=false;
	

	*run {
		done = true;
		objects.do({ arg item; item.doOnStartUp  });
		// "StartUp done.".postln;
	}


	*defer { arg object;
		 if(done) { object.doOnStartUp } { this.add(object) }
	}

	

}

AbstractServerAction : AbstractSystemAction {
	
	*performFunction { arg server, function;
		this.objects.at(server).do(function)
	}
	
	*run { arg server;
		var selector = this.functionSelector;
		// selector.postln;
		this.performFunction(server, { arg obj; obj.perform(selector) });
	}
	
	*functionSelector {
		^this.subclassResponsibility(thisMethod)
	}
	
	*add { arg object, server;
		
		server = server ? Server.default;

		if (this.objects.includesKey(server).not) {
			this.objects.put(server, List.new)
		};
		
		if (this.objects.at(server).includes(object).not) {
			this.objects.at(server).add(object)
		};

	}
	
	*addToAll { arg object;
		
		Server.set.do({ arg s; this.add(object, s) });
		
	}
	
	*remove { arg object, server;
		
		server = server ? Server.default;

		if ( this.objects.includesKey(server) ) {
			this.objects.at(server).remove(object)
		};
		
	}
	
}

// things to do after server has booted


ServerBoot : AbstractServerAction {

	classvar <>objects;
	
	*initClass {
		this.objects = IdentityDictionary.new;
	}
	
	*functionSelector {
		^\doOnServerBoot
	}

}

// things to do after server has quit


ServerQuit : AbstractServerAction {

	classvar <>objects;
	
	*initClass {
		this.objects = IdentityDictionary.new;
	}

	*functionSelector {
		^\doOnServerQuit
	}

}


// things to do after server has booted and initialised


ServerTree : AbstractServerAction {


	classvar <>objects;


	*initClass {
		this.objects = IdentityDictionary.new;
	}

	*functionSelector {
		^\doOnServerTree
	}
	
}


// things to do before system shuts down

ShutDown : AbstractSystemAction {

	classvar <>objects;

	*initClass {
		UI.registerForShutdown({ this.run });
	}

	*run {
		objects.do({ arg item; item.doOnShutDown;  });
	//	"ShutDown done.".postln;
	}

}

// this is still not solved. But if a solution is found, it can be done here:

WasAsleep : AbstractSystemAction {
	classvar <sleeping = false;
	
	*initClass {
		var platform = thisProcess.platform;
		Platform.case(\osx, {
			platform.sleepAction = platform.sleepAction.addFunc { this.goToSleep };
			platform.wakeAction = platform.wakeAction.addFunc { this.awake };
		});
	}
	
	
	*goToSleep {
		sleeping = true;
	}
	
	*awake {
		sleeping = false;
	}
	
	
}

