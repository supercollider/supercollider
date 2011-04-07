+ Main
{
	platformClass { ^OSXPlatform }

	applicationStart {
		ApplicationStart.run;
	}

	preferences {
		this.platform.preferencesAction.value(this)
	}
	preferencesAction_ { arg f;
		this.platform.preferencesAction_(f)
	}
	preferencesAction {
		^this.platform.preferencesAction
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
