// These Streams are instantiated by math operations on other Streams

UnaryOpStream : Stream {
	var >operator, >a;

	*new { |operator, a|
		^super.newCopyArgs(operator, a)
	}
	next { |inval|
		var vala;
		vala = a.next(inval);
		if(vala.isNil) { ^nil } { ^vala.perform(operator) }
	}
	reset { a.reset }
	storeOn { |stream| stream <<< a << "." << operator }

}

BinaryOpStream : Stream {
	var >operator, >a, >b;

	*new { |operator, a, b|
		^super.newCopyArgs(operator, a, b)
	}
	next { |inval|
		var vala, valb;
		vala = a.next(inval);
		if(vala.isNil) { ^nil };
		valb = b.next(inval);
		if(valb.isNil) { ^nil };
		^vala.perform(operator, valb)
	}
	reset { a.reset; b.reset }

	storeOn { |stream|
		stream << "(" <<< a << " " << operator.asBinOpString << " " <<< b << ")"
	}
}


BinaryOpXStream : Stream {
	var operator, a, b, vala;

	*new { |operator, a, b|
		^super.newCopyArgs(operator, a, b)
	}
	next { |inval|
		var valb;
		if(vala.isNil) {
			vala = a.next(inval);
			if(vala.isNil) { ^nil };
			valb = b.next(inval);
			if(valb.isNil) { ^nil }
		} {
			valb = b.next(inval);
			if(valb.isNil) {
				vala = a.next(inval);
				if(vala.isNil) { ^nil };
				b.reset;
				valb = b.next(inval);
				if(valb.isNil) { ^nil }
			}
		};
		^vala.perform(operator, valb)
	}
	reset { vala = nil; a.reset; b.reset }
	storeOn { |stream|
		stream << "(" <<< a << " " << operator.asBinOpString;
		stream << ".x";
		stream << " " <<< b << ")"
	}
}



NAryOpStream : Stream {
	var >operator, >a, arglist;
	var isNumeric;

	*new { |operator, a, arglist|
		^super.newCopyArgs(operator, a).arglist_(arglist)
	}
	arglist_ { |list|
		// optimization
		isNumeric = list.every({ |item| item.isNumber or: { item.class === Symbol } }); 		arglist = list
	}
	next { |inval|
		var vala, values;
		vala = a.next(inval);
		if(vala.isNil) { ^nil };
		values = if(isNumeric) { arglist } {
			arglist.collect { |item|
				var res;
				res = item.next(inval);
				if(res.isNil) { ^nil };
				res
			}
		};
		^vala.performList(operator, values)
	}
	reset { a.reset; arglist.do { |item| item.reset } }

	storeOn { |stream| stream <<< a << "." << operator << "(" <<<* arglist << ")" }
}
