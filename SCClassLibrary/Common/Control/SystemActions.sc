AbstractSystemAction {

	*init {
		this.objects = List.new
	}

	*add { |object|
		if(this.objects.isNil) { this.init }; // lazy init
		if(this.objects.includes(object).not) { this.objects.add(object) }
	}

	*remove { |object|
		this.objects.remove(object)
	}

	*removeAll {
		this.init
	}


	*objects { ^this.shouldNotImplement(thisMethod) }
	*objects_ { |obj| ^this.shouldNotImplement(thisMethod) }
}



// things to clear when hitting cmd-.

CmdPeriod : AbstractSystemAction {
	classvar <>objects;
	classvar <era = 0;
	classvar <>clearClocks = true;
	classvar <>freeServers = true;
	classvar <>freeRemote = false;



	*doOnce { |object|
		var f = { this.remove(f); object.doOnCmdPeriod };
		this.add(f)
	}
	*run {
		if(clearClocks) {
			SystemClock.clear;
			AppClock.clear
		};

		objects.copy.do { |item| item.doOnCmdPeriod };

		if(freeServers) {
			Server.freeAll(freeRemote); // stop all sounds on local, or remote servers
			Server.resumeThreads
		};

		era = era + 1

	}

	*hardRun {

		SystemClock.clear;
		AppClock.clear;
		TempoClock.default.clear;

		objects.copy.do { |item| item.doOnCmdPeriod };



		Server.hardFreeAll; // stop all sounds on local servers
		Server.resumeThreads;
		era = era + 1

	}


}


// things to do after startup file executed

StartUp : AbstractSystemAction {


	classvar <>objects, <done = false;


	*run {
		done = true;
		objects.copy.do { |item| item.doOnStartUp }
		// "StartUp done.".postln;
	}


	*defer { |object|
		if(done) { object.doOnStartUp } { this.add(object) }
	}


}


// things to do before system shuts down

ShutDown : AbstractSystemAction {

	classvar <>objects;

	*run {
		objects.copy.do { |item| item.doOnShutDown }
		//	"ShutDown done.".postln;
	}

}

// things to do on a system reset
OnError : AbstractSystemAction {
	classvar <>objects;

	*run {
		objects.copy.do { |item| item.doOnError }
	}
}


AbstractServerAction : AbstractSystemAction {

	*init {
		this.objects = IdentityDictionary.new
	}

	*performFunction { |server, function|
		if(this.objects.notNil) {
			this.objects.at(server).copy.do(function);
			if(server === Server.default) {
				this.objects.at(\default).copy.do(function)
			};
			this.objects.at(\all).copy.do(function)
		}
	}

	*run { |server|
		var selector = this.functionSelector;
		// selector.postln;
		this.performFunction(server, { |obj| obj.perform(selector, server) })
	}

	*functionSelector {
		^this.subclassResponsibility(thisMethod)
	}

	*add { |object, server|
		var list;
		if(server.isNil) { server = \all };
		if(this.objects.isNil) { this.init };
		list = this.objects.at(server);
		if(list.isNil) { list = List.new; this.objects.put(server, list) };
		if(list.includes(object).not) { list.add(object) }
	}

	*addToAll { |object|
		this.add(object, \all)
	}

	*remove { |object, server|
		if(server.isNil) { server = \all };
		this.objects !? { this.objects.at(server).remove(object) }
	}

	*removeServer { |server|
		this.objects.removeAt(server)
	}
}

// things to do after server has booted


ServerBoot : AbstractServerAction {

	classvar <>objects;

	*functionSelector {
		^\doOnServerBoot
	}
}

// things to do after server has quit


ServerQuit : AbstractServerAction {

	classvar <>objects;

	*functionSelector {
		^\doOnServerQuit
	}
}


// things to do after server has booted and initialised


ServerTree : AbstractServerAction {

	classvar <>objects;

	*functionSelector {
		^\doOnServerTree
	}
}
