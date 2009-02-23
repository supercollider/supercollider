

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


AbstractServerAction : AbstractSystemAction {
	
	*init {
		this.objects = IdentityDictionary.new;
	}
	
	*performFunction { arg server, function;
		this.objects.at(server).do(function);
		if(server === Server.default) {
			this.objects.at(\default).do(function)
		};
		this.objects.at(\all).do(function);
	}
	
	*run { arg server;
		var selector = this.functionSelector;
		// selector.postln;
		this.performFunction(server, { arg obj; obj.perform(selector, server) });
	}
	
	*functionSelector {
		^this.subclassResponsibility(thisMethod)
	}
	
	*add { arg object, server;
		var list;
		if(server.isNil) { server = \default };
		list = this.objects.at(server);
		if(list.isNil) { list = List.new; this.objects.put(server, list) };
		if (list.includes(object).not) { list.add(object) };

	}
	
	*addToAll { arg object;
		
		Server.set.do({ arg s; this.add(object, s) });
		
	}
	
	*remove { arg object, server;
		
		if(server.isNil) { server = \default };
		this.objects.at(server).remove(object);
		
	}
	
	*removeServer { arg server;
		this.objects.removeAt(server)
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

