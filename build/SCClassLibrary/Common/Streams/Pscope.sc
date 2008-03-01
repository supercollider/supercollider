Pscope : FilterPattern {
	var <>scope;
	
	*new { arg pattern, scope;
		^super.new(pattern).scope_(scope)
	}
	
	embedInStream { arg inval;
		var eventScope, embedScope, stream, outval, parentScope;
		eventScope = ();
		scope !? {
			scope.keysValuesDo { |key, val|
				eventScope[key] = Ref(val)
			}
		};
		
		stream = pattern.asStream;
		parentScope = inval[\eventScope];
		embedScope = eventScope.copy.parent_(parentScope);
		
		while {
			inval = inval.copy;
			inval[\eventScope] = embedScope;
			outval = stream.next(inval);
			outval.notNil
		} {
			inval = outval.yield
		};
		
		^inval
	}

}

Plet : FilterPattern {
	var <>key;
	*new { arg key, pattern;
		^super.new(pattern).key_(key)
	}
	
	embedInStream { arg inval;
		var str = pattern.asStream, val, sval, outval, scope;
		while {
			val = str.next(inval);
			val.notNil
		} {
			scope = inval[\eventScope];
			if(scope.isNil) { Error("no scope defined in event").throw };
			outval = scope[key];
			if(sval.isNil) { scope[key] = sval = Ref.new };
			sval.value = val;
			inval = val.yield;
		}
	}

}


Pget : Pattern {
	var <>key, <>default, <>repeats;
	
	*new { arg key, default, repeats = 1;
		^super.newCopyArgs(key, default, repeats)
	}
	
	embedInStream { arg inval;
			var scope, outval;
			repeats.value.do {
				scope = inval[\eventScope];
				if(scope.isNil) { Error("no scope defined in event").throw };
				inval = (scope[key].value ? default).yield;
			};
			^inval
	}

}

