+ Server {
	onBootAdd { arg object;
		ServerBoot.add(object, this);
	}
	onBootRemove { arg object;
		ServerBoot.remove(object, this);
	}
	onQuitAdd { arg object;
		ServerQuit.add(object, this);
	}
	onQuitRemove { arg object;
		ServerQuit.remove(object, this);
	}
}

+ Function {

	doOnStartUp { this.value }
	doOnCmdPeriod { this.value }
	doOnShutDown { this.value }
	doOnError { this.value }
	doOnServerBoot { arg server; this.value(server) }
	doOnServerQuit { arg server; this.value(server) }
	doOnServerTree { arg server; this.value(server) }

}

+ Object {
	doOnCmdPeriod { ^this.cmdPeriod } // compatibility
}
