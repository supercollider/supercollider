+ Server {
	onBootAdd { |object|
		ServerBoot.add(object, this)
	}
	onBootRemove { |object|
		ServerBoot.remove(object, this)
	}
	onQuitAdd { |object|
		ServerQuit.add(object, this)
	}
	onQuitRemove { |object|
		ServerQuit.remove(object, this)
	}
}

+ Function {

	doOnStartUp { this.value }
	doOnCmdPeriod { this.value }
	doOnShutDown { this.value }
	doOnError { this.value }
	doOnServerBoot { |server| this.value(server) }
	doOnServerQuit { |server| this.value(server) }
	doOnServerTree { |server| this.value(server) }

}

+ Object {
	doOnCmdPeriod { ^this.cmdPeriod } // compatibility
}
