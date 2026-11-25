Association : Magnitude {
	var <>key, <>value;

	*new { |key, value|
		^super.newCopyArgs(key, value)
	}

	== { |anAssociation|
		^anAssociation respondsTo: \key and: { key == anAssociation.key }
	}

	hash {
		^key.hash
	}

	< { |anAssociation|
		^key < anAssociation.key
	}

	printOn { |stream|
		stream << "(" << key << " -> " << value << ")"
	}
	storeOn { |stream|
		stream << "(" <<< key << " -> " <<< value << ")"
	}

	// Pattern support
	embedInStream { |inval|
		^inval.add(this).yield
	}
	transformEvent { |event|
		^event.add(this)
	}
}
