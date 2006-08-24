// These reimplement methods already defined for Streams.
// Because of the redundancy, they might not be included.
+Punop {
	
	embedInStream { arg inval;
		var stream, outval;
		stream = a.asStream;
		loop { 
			outval = stream.next;
			if (outval.isNil) { ^inval };
			inval = yield(outval.perform(operator));
		}
	}
}

+Pbinop  {

	embedInStream { arg inval;
		var streamA, streamB, vala, valb;
		streamA = a.asStream;
		streamB = b.asStream;
		if (adverb.isNil) {
			loop {
				vala = streamA.next;
				if (vala.isNil) { ^inval };
				valb = streamB.next;
				if (valb.isNil) { ^inval };
				inval = yield(vala.perform(operator, valb));
			};
		};
		if (adverb == 'x') {
			if (vala.isNil) { 
				vala = a.next;
				if (vala.isNil) { ^inval };
				valb = b.next;
				if (valb.isNil, { ^inval });
			}{
				valb = b.next;
				if (valb.isNil) { 
					vala = a.next;
					if (vala.isNil) { ^inval };
					b.reset;
					valb = b.next;
					if (valb.isNil) { ^inval };
				};
			};
			^inval = yield(vala.perform(operator, valb));
		};

	}
}

+Pnaryop {
	embedInStream { arg inval;
		var streamA, streamlist, vala, values, isNumeric;
		streamA = a.asStream;
		isNumeric = arglist.every { arg item; 
			item.isNumber or: {item.class === Symbol} }; // optimization
		
		if (isNumeric) {
			loop {
				vala = streamA.next(inval);
				if (vala.isNil) { ^inval };
				inval = yield(vala.performList(operator, arglist));
			}
		}{		
			streamlist = arglist.collect({ arg item; item.asStream });
			loop {
				vala = streamA.next(inval);
				if (vala.isNil) { ^inval };
				values = streamlist.collect({ arg item;
					var result = item.next;
					if (result.isNil) { ^inval };
					result
				});
				inval = yield(vala.performList(operator, values));
			}
		};
	}
}

+Pcollect {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			outval = stream.next(inval);
			if (outval.isNil) { ^inval };
			inval = yield(func.value(outval));
		}
	}
}

+Pselect {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			while ({ 
				outval = stream.next(inval);
				if (outval.isNil) { ^inval };
				func.value(outval).not 
			});
			inval = yield(outval);
		}
	}
}

+Preject {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			while ({ 
				outval = stream.next(inval);
				if (outval.isNil) { ^inval };
				func.value(outval); 
			});
			inval = yield(outval);
		}
	}
}