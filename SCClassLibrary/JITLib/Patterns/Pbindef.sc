
PbindProxy : Pattern {

	var <>pairs, <source;

	*new { | ... pairs |
		^super.newCopyArgs(pairs).init
	}

	init {
		forBy(0, pairs.size-1, 2) { |i|
			var proxy = PatternProxy.new;
			proxy.setSource(pairs[i+1]);
			pairs[i+1] = proxy
		};
		source = EventPatternProxy(Pbind(*pairs))
	}

	clear {
		pairs = [];
		source.clear;
	}

	embedInStream { |inval|
		^source.embedInStream(inval)
	}

	quant_ { |val|
		pairs.pairsDo { |key, item| item.quant = val }; // maybe use ref later
		source.quant = val;
	}

	quant { ^source.quant }

	envir { ^source.envir }

	envir_ { |envir|
		source.envir_(envir)
	}

	find { |key|
		pairs.pairsDo { |u, x, i|
			if(u == key) { ^i }
		};
		^nil
	}

	at { |key|
		var i = this.find(key);
		^if(i.notNil) { pairs[i+1] } { nil }
	}

	set { | ... args | // key, val ...
		this.setPatternPairs(args)
	}

	setPatternPairs { | newPairs, stitch = true |
		// newPairs are modified in place. If necessary, copy before passing it here.
		var toRemove, toRemoveInArgs, changed = false;
		var quant = this.quant;


		forBy(0, newPairs.lastIndex, 2, { |i|

			var val = newPairs[i + 1];
			var key = newPairs[i];
			var j = this.find(key);
			var proxy;

			if(val.notNil) {
				// find existing or make a new one
				proxy = if(j.notNil) { pairs[j + 1] } { PatternProxy.new };
				proxy.setSource(val);
				newPairs[i + 1] = proxy; // replace argument value by proxy
			} {
				// add index of key and value to remove-list
				toRemoveInArgs = toRemoveInArgs.add(i).add(i + 1);
				if(j.notNil) { toRemove = toRemove.add(j).add(j + 1) };
			}

		});

		// now remove all those which were nil
		toRemove !? { changed = true; pairs = pairs.reject { |x, i| toRemove.includes(i) } };
		toRemoveInArgs !? { newPairs = newPairs.reject { |x, i| toRemoveInArgs.includes(i) } };

		// stitch old pairs into new pairs
		if(stitch) { newPairs = pairs.stitchIntoPairs(newPairs) };

		if(changed or: { newPairs != pairs }) {
			pairs = newPairs;
			pairs.pairsDo { |key, x| x.quant = quant };
			source.source = Pbind(*pairs)
		}
	}

	storeArgs {
		var result = Array(pairs.size);
		pairs.pairsDo { |key, value|
			result.add(key).add(value.source)
		};
		^result
	}
}


Pbindef : Pdef {

	*new { | key ... pairs |

		var pat = super.new(key);
		var src = pat.source;

		if(pairs.isEmpty.not) {
			if(src.isKindOf(PbindProxy)) {
				src.setPatternPairs(pairs, this.stitch);
				pat.wakeUp;
			} {
				if(src.isKindOf(Pbind))
				{
					src.patternpairs.pairsDo { |key, pat|
						if(pairs.includes(key).not) {
							pairs = pairs.add(key);
							pairs = pairs.add(pat);
						}
					}
				};

				src = PbindProxy.new(*pairs).quant_(pat.quant);
				pat.source = src
			};
		};

		^pat

	}

	*stitch { ^true }

	storeArgs { ^[key] ++ pattern.storeArgs }

	repositoryArgs { ^this.storeArgs }

	quant_ { |val|
		super.quant = val;
		source.quant = val
	}

	*hasGlobalDictionary { ^true }

}

PbindefCut : Pbindef {

	*stitch { ^false }

}
