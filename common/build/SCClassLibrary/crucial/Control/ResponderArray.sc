
ResponderArray {
	var <>array;
	*new { arg array;
		^super.new.array_(array)
	}
	add { arg thing;
		array = array.add(thing)
	}
	respond { arg ... vals;
		^array.any({ |r| r.performList(\respond,vals) })
	}
	// match ?
	value { arg ... vals;
		array.do({ arg resp; resp.performList(\value,vals) })
	}
	remove { arg resp;
		array.remove(resp);
		^array.size
	}
}

