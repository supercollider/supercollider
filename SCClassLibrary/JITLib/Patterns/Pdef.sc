
Pdefn : PatternProxy {
	var <key;
	classvar <>all;

	*initClass {
		all = IdentityDictionary.new;
	}
	*new { arg key, item;
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	map { arg ... args;
		if(envir.isNil) { this.envir = () };
		args.pairsDo { |key, name| envir.put(key, Pdefn(name)) };
		this.changed(\map, args);
	}

	storeArgs { ^[key] } // assume it was created globally
	copy { |toKey|
		if(key == toKey) { Error("cannot copy to identical key").throw };
		^this.class.new(toKey).copyState(this)
	}

	prAdd { arg argKey;
		key = argKey;
		all.put(argKey, this);
	}

	*hasGlobalDictionary { ^true }

}



Tdef : TaskProxy {
	var <key;
	classvar <>all;

	*initClass {
		all = IdentityDictionary.new;
	}

	*new { arg key, item;
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	storeArgs { ^[key] }

	copy { |toKey|
		if(key == toKey) { Error("cannot copy to identical key").throw };
		^this.class.new(toKey).copyState(this)
	}

	prAdd { arg argKey;
		key = argKey;
		all.put(argKey, this);
	}

	*hasGlobalDictionary { ^true }

}


Pdef : EventPatternProxy {
	var <key;

	classvar <>all;

	storeArgs { ^[key] }

	*new { arg key, item;
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	map { arg ... args;
		if(envir.isNil) { this.envir = () };
		args.pairsDo { |key, name| envir.put(key, Pdefn(name)) }
	}

	prAdd { arg argKey;
		key = argKey;
		all.put(argKey, this);
	}

	copy { |toKey|
		if(key == toKey) { Error("cannot copy to identical key").throw };
		^this.class.new(toKey).copyState(this)
	}

	*hasGlobalDictionary { ^true }

	*initClass {
		var phraseEventFunc;

		all = IdentityDictionary.new;
		Class.initClassTree(Event);

		phraseEventFunc = {
			var pat, event, outerEvent, recursionLevel, instrument, embeddingLevel, freq, rest;

			embeddingLevel = ~embeddingLevel ? 0; // infinite recursion catch
			freq = ~freq.value;
			rest = freq.isKindOf(Symbol); // check for outer rests
			if(rest) { ~freq = freq };
			pat = (~repository ? all).at(~instrument);

			if(pat.notNil and: { embeddingLevel < 8 })
			{
				pat = pat.pattern; // optimization. outer pattern takes care for replacement
				// preserve information from outer pattern, but not delta.

				recursionLevel = ~recursionLevel;
				if(~transparency.isNil or:
					{ ~transparency > (recursionLevel ? 0) }
				) {
					outerEvent = currentEnvironment.copy
				} {
					outerEvent = Event.default;
					outerEvent.use {
						~type = \phrase;
						~recursionLevel = recursionLevel;
					}
				};

				if(recursionLevel.notNil) {
					if(recursionLevel > 0) {
						// in recursion, some inner values have to be overridden
						instrument = ~instrument;
						pat = pat.collect { |inval|
							inval.use {
								~instrument = instrument;
								~parent = outerEvent;
								~recursionLevel = recursionLevel - 1;
							};
							inval
						};
					} {
						// play pattern in the ordinary way
						~type = \note;
					};
				} {	// avoid recursion, if instrument not set.
					outerEvent.put(\embeddingLevel, embeddingLevel + 1);
					outerEvent.parent_(Event.parentEvents.default);
				};
				// maybe add a Pprotect here.
				// pat.asProtected
				pat = Pfindur(~sustain.value, pat);
				outerEvent.put(\delta, nil); // block delta modification by Ppar
				outerEvent.put(\instrument, ~synthDef ? \default);

				pat.play(thisThread.clock, outerEvent, 0.0);
			} {
				~type = \note;
				~play.value;
			}

		};

		Event.addEventType(\phrase, phraseEventFunc);
	}

}

