// These Streams are instantiated by math operations on other Streams

UnaryOpStream : Stream {
	var >operator, >a;

	*new { arg operator, a;
		^super.newCopyArgs(operator, a)
	}
	next { arg inval;
		var vala;
		vala = a.next(inval);
		if (vala.isNil, { ^nil },{ ^vala.perform(operator); });
	}
	reset { a.reset }
	storeOn { arg stream; stream <<< a << "." << operator }

}

BinaryOpStream : Stream {
	var >operator, >a, >b;

	*new { arg operator, a, b;
		^super.newCopyArgs(operator, a, b)
	}
	next { arg inval;
		var vala, valb;
		vala = a.next(inval);
		if (vala.isNil, { ^nil });
		valb = b.next(inval);
		if (valb.isNil, { ^nil });
		^vala.perform(operator, valb);
	}
	reset { a.reset; b.reset }

	storeOn { arg stream;
			stream << "(" <<< a << " " << operator.asBinOpString << " " <<< b << ")"
	}
}


BinaryOpXStream : Stream {
	var operator, a, b, vala;

	*new { arg operator, a, b;
		^super.newCopyArgs(operator, a, b)
	}
	next { arg inval;
		var valb;
		if (vala.isNil) {
			vala = a.next(inval);
			if (vala.isNil) { ^nil };
			valb = b.next(inval);
			if (valb.isNil, { ^nil });
		}{
			valb = b.next(inval);
			if (valb.isNil) {
				vala = a.next(inval);
				if (vala.isNil) { ^nil };
				b.reset;
				valb = b.next(inval);
				if (valb.isNil) { ^nil };
			};
		};
		^vala.perform(operator, valb);
	}
	reset { vala = nil; a.reset; b.reset }
	storeOn { arg stream;
			stream << "(" <<< a << " " << operator.asBinOpString;
			stream << ".x";
			stream << " " <<< b << ")"
	}
}



NAryOpStream : Stream {
	var >operator, >a, arglist;
	var isNumeric;

	*new { arg operator, a, arglist;
		^super.newCopyArgs(operator, a).arglist_(arglist)
	}
	arglist_ { arg list;
		// optimization
		isNumeric = list.every({ arg item; item.isNumber or: {item.class === Symbol} }); 		arglist = list;
	}
	next { arg inval;
		var vala, values;
		vala = a.next(inval);
		if (vala.isNil, { ^nil });
		values = if (isNumeric) { arglist } {
				arglist.collect({ arg item; var res;
					res = item.next(inval);
					if(res.isNil) { ^nil };
					res
				})
				};
		^vala.performList(operator, values);
	}
	reset { a.reset; arglist.do({ arg item; item.reset }) }

	storeOn { arg stream; stream <<< a << "." << operator << "(" <<<* arglist << ")" }
}
