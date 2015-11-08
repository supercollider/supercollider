+ Main {
	platformClass { ^OSXPlatform }

	applicationStart {
		ApplicationStart.run;
	}

	sleep {
		this.platform.isSleeping = true;
		^this.platform.sleepAction.value(this);
	}
	wake {
		this.platform.isSleeping = false;
		^this.platform.wakeAction.value(this);
	}
	isSleeping { ^this.platform.isSleeping }
}
