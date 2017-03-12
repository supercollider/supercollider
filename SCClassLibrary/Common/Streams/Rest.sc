

Rest : Operand  {

	dur_ { |dt| value = dt }
	dur { ^value }

	// EVENT SUPPORT
	asControlInput { ^value ? 1 }
	playAndDelta { ^value ? 1 }
	isRest { ^true }

	prescribeRest { |event|
		event.put(\isRest, true)
	}


}

+ Object {

	prescribeRest {}

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
