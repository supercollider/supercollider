QCallback : QObject {
	var <action;

	*qtClass { ^'QtCollider::QcCallback'; }

	*new {
		|func|
		^super.new.action_(func)
	}

	*newFrom {
		|other|
		if (other.isKindOf(QCallback)) {
			^QCallback(other.action)
		} {
			^QCallback(other)
		}
	}

	action_{
		|newAction|
		if (newAction != action) {
			this.prOnCalledSignals.do({
				|signal|
				if (action.notNil) { this.disconnectFunction(signal.asSymbol, action) };
				if (newAction.notNil) { this.connectFunction(signal.asSymbol, { |func, v| newAction.(v) }, true) }
			});
			action = newAction;
		}
	}

	value {
		|...args|
		action.value(*args)
	}

	prOnCalledSignals {
		^this.methods(false, true, false).select({
			|sig|
			sig.asString.find("onCalled").notNil
		})
	}
}
