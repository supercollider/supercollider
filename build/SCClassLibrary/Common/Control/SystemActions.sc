

AbstractSystemAction {

	*init {
		this.objects = List.new;
	}
	
	*add { arg object;
		if(this.objects.isNil) { this.init }; // lazy init
	//	if(this.objects.includes(object).not) { // agree on this
			this.objects.add(object)
	//	}
	}
	
	*remove { arg object;
		this.objects.remove(object)
	}

	*objects { ^this.shouldNotImplement(thisMethod) }
	*objects_ { arg obj; ^this.shouldNotImplement(thisMethod) }
}


// things to clear when hitting cmd-.CmdPeriod : AbstractSystemAction {	classvar <>objects;	classvar <era = 0;	classvar <>clearClocks = true;	classvar <>freeServers = true;
	classvar <>freeRemote = false;

	*doOnce { arg object;		var f = { this.remove(f); object.doOnCmdPeriod  };		this.add(f);	}	*run {
		if(clearClocks, {			SystemClock.clear;			AppClock.clear;	//		TempoClock.default.clear;		});	
		objects.do({ arg item; item.doOnCmdPeriod;  });
	
		if(freeServers, {			Server.freeAll(freeRemote); // stop all sounds on local, or remote servers			Server.resumeThreads;		});	
		era = era + 1;
	}	*hardRun {
		SystemClock.clear;		AppClock.clear;		TempoClock.default.clear;		objects.do({ arg item; item.doOnCmdPeriod;  });
				Server.freeAll; // stop all sounds on local servers		Server.resumeThreads;		era = era + 1;
	}}// things to do after startup file executedStartUp : AbstractSystemAction {	classvar <>objects, <done=false;		*run {		done = true;		objects.do({ arg item; item.doOnStartUp;  });
		"StartUp done.".postln;	}	*defer { arg object;		 if(done) { object.doOnStartUp } { this.add(object) }	}	}ServerBoot : AbstractSystemAction {	classvar <>objects;
	
	*initClass {
		this.objects = IdentityDictionary.new;
	}
	*add { arg object, server;
		
		if (server.isNil) {
		
			Server.set.do({ arg s; this.add(object, s) });
		
		}{
		
			if (this.objects.includesKey(server).not) {
				this.objects.put(server, List.new);
			};
		
		//	if ( this.objects.at(server).includes(object).not ) {
				this.objects.at(server).add(object)
		//	};
		
		}
	}
	
	
	
	*remove { arg object, server;
		
		if (server.isNil) {
			Server.set.do({ arg s; this.remove(object, s) });
		}{
			if (this.objects.includesKey(server)) {
				this.objects.at(server).remove(object)
			};
		};
		
	}

	
	*run { arg server;		this.objects.at(server).do({ arg item; item.doOnServerBoot(server)  });		//postf("% ServerBoot\n", server);
	}

}ServerQuit : ServerBoot {	classvar <>objects;		*initClass {
		this.objects = IdentityDictionary.new;
	}

	*run { arg server;
		this.objects.at(server).do({ arg item; item.doOnServerQuit(server)  });
		//postf("% ServerQuit\n", server);
	}
	}

ServerTree : ServerBoot {

	classvar <>objects;


	*initClass {
		this.objects = IdentityDictionary.new;
	}

	*run { arg server;
		this.objects.at(server).do({ arg item; item.doOnServerTree(server)  });
		//postf("% ServerTree\n", server);
	}
	
}
ShutDown : AbstractSystemAction {	classvar <>objects;	*initClass {
		UI.registerForShutdown({ this.run });
	}
	*run {		objects.do({ arg item; item.doOnShutDown;  });
	//	"ShutDown done.".postln;	}}

