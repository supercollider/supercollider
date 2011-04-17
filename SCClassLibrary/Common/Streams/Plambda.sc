Plambda : FilterPattern {
	var <>scope;

	*new { arg pattern, scope;
		^super.new(pattern).scope_(scope)
	}

	embedInStream { arg inval;
		var embedScope, stream, outval, parentScope;

		stream = pattern.asStream;
		inval !? { parentScope = inval[\eventScope] };
		embedScope = (scope.copy ? ()).parent_(parentScope);

		while {
			inval = inval.copy ? ();
			inval[\eventScope] = embedScope;
			outval = stream.next(inval);
			outval.notNil
		} {
			// return outer scope
			outval = outval.copy;
			outval[\eventScope] = outval[\eventScope].eventAt(\parent);
			inval = outval.yield
		};

		^inval
	}
	storeArgs {
		^if(scope.notNil) { [pattern, scope] } { [pattern] }
	}

}

Plet : Pattern {
	var <>pattern, <>key, <>return;
	*new { arg key, pattern, return;
		^super.newCopyArgs(pattern, key, return)
	}

	embedInStream { arg inval;
		var str = pattern.asStream, val, sval, outval, scope;
		var returnStr = return.asStream, returnVal;
		while {
			outval = str.next(inval);
			returnVal = returnStr.next(inval);
			outval.notNil
		} {
			scope = inval[\eventScope];
			if(scope.isNil) { Error("no scope defined in event").throw };

			// don't transmit scope
			val = outval.copy;
			if(val.eventAt(\eventScope).notNil) { val[\eventScope] = nil };
			scope[key] = val;

			inval = (returnVal ? outval).yield;
		}
	}

	silent { return = Event.silent }

	storeArgs { ^[key, pattern] ++ return }

}


Pget : Pattern {
	var <>key, <>default, <>repeats;

	*new { arg key, default, repeats = 1;
		^super.newCopyArgs(key, default, repeats)
	}

	embedInStream { arg inval;
			var scope = inval[\eventScope], outval;
			if(scope.isNil) { Error("no scope defined in event").throw };
			repeats.value(inval).do {
				outval = scope[key] ? default;
				if(outval.isNil) { ^inval };
				outval.yield
			};
			^inval
	}

	storeArgs {
		var list = [key];
		if(repeats != 1) { ^[key, default, repeats] };
		if(default.notNil) { ^[key, default] }
		^[key]
	}

}
