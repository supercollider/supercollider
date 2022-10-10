
Rest : Operand  {

	*new { |value = 1|
		^super.newCopyArgs(value.dereferenceOperand).unwrapBoolean
	}

	dur_ { |dt| value = dt }
	dur { ^value.value }

	// COMPARISONS JUST WORK
	unwrapBoolean {
		^if(value.isKindOf(Boolean)) { value } { this }
	}

	// EVENT SUPPORT
	asControlInput { ^value.asControlInput }
	playAndDelta { ^value.value }
	isRest { ^true }

	// DEPRECATION
	*embedInStream { |inval|
		"Please use \"Rest()\" instances instead of the \"Rest\" class".warn;
		^this.new.embedInStream(inval);
	}

	*asStream {
		"Please use \"Rest()\" instances instead of the \"Rest\" class".warn;
		^this.new
	}

	asEvent { ^Event.silent(this) }
}

+ SimpleNumber {
	// Some patterns call .delta on the eventstream's yield value
	// since a SimpleNumber is a valid rest, it must answer 'delta' with itself
	delta {}
	// but Ppar and several other patterns do "put(\delta, ...)"
	// so they need to convert the simplenumber into a real rest event
	asEvent { ^Event.silent(this) }
}

+ Event {
	asEvent {}
}

+ Nil {
	// Ppar etc. need stream.next(event).asEvent to be nil when the stream ends
	asEvent {}
}
