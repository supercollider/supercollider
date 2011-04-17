+ Spec {
	*guess { |key, value|
		^if (value.abs > 0) {
			[value/20, value*20, \exp].asSpec
		} {
			[-2, 2, \lin].asSpec
		};
	}

	*suggestString { |key, value|
		^"Spec.add(" + this.guess.storeArgs + ");"
	}
}

+ Dictionary {

	softPut { |param, val, within = 0.025, mapped = true, lastVal, spec|
		var curVal, curValNorm, newValNorm, lastValNorm, maxDiff;

		curVal = this.at(param);
		spec = (spec ? param).asSpec;

		if (curVal.isNil or: spec.isNil) {
			this.put(param, val);
			^true
		};

		curValNorm = spec.unmap( curVal );
		maxDiff = max(within, spec.step);

		if (mapped) {
			newValNorm = spec.unmap(val);
			if (lastVal.notNil) { lastValNorm = spec.unmap(lastVal) };
		} {
			newValNorm = val;
			lastValNorm = lastVal;
			val = spec.map(val);
		};

		if (
			(newValNorm.absdif(curValNorm) <= maxDiff)   // new val is close enough
									// or controller remembers last value it sent.
			or: { lastValNorm.notNil and: { curValNorm.absdif(lastValNorm) <= maxDiff } })
		{
			this.put(param, val);
			^true
		} {
			^false
		}
	}

	softSet { |param, val, within = 0.025, mapped = true, lastVal, spec|
		this.softPut(param, val, within, mapped, lastVal, spec);
	}
}

+ PatternProxy {

	softSet { |param, val, within = 0.025, mapped = true, lastVal, spec|
		if(envir.isNil) {
			if (mapped.not) {
				spec = (spec ? param).asSpec;
				val = spec.map(val);
				^this.set(param, val)
			}
		} {
			^this.envir.softPut(param, val, within, lastVal, mapped, spec)
		}
	}

}


+ NodeProxy {

	get { |param|
		^this.nodeMap.get(param).value ?? { this.getDefaultVal(param) };
	}

	getDefaultVal { |key|
		this.objects.do { |obj|
			obj.controlNames.do { |ctlname|
				if (ctlname.name == key) { ^ctlname.defaultValue }
			}
		};
		^nil
	}
			// must have a spec
	nudgeSet { |param, incr = 0.02, spec|
		var curValNorm, newValNorm;
		spec = (spec ? param).asSpec;

		curValNorm = spec.unmap( this.get(param) );
		newValNorm = (curValNorm + incr).clip(0, 1);
		this.set(param, spec.map(newValNorm));
	}

	nudgeVol { |incr = 0.02, spec|
		var curVolNorm, newVolNorm;
		spec = (spec ? \amp).asSpec;

		curVolNorm = spec.unmap(this.vol);
		newVolNorm = (curVolNorm + incr).clip(0, 1);
		this.vol_(spec.map(newVolNorm))
	}

	softSet { |param, val, within = 0.025, mapped = true, lastVal, spec|
		var curVal, curValNorm, newValNorm, maxDiff, hasLast, lastValNorm;

		spec = (spec ? param).asSpec;
		curVal = this.get(param);

		if (curVal.isNil or: spec.isNil) {
			this.set(param, val);
			^true
		};

		curValNorm = spec.unmap( curVal );

		maxDiff = max(within, spec.step);
		hasLast = lastVal.notNil;

		if (mapped) {
			newValNorm = spec.unmap(val);
			if (hasLast) { lastValNorm = spec.unmap(lastVal) };
		} {
			newValNorm = val;
			val = spec.map(val);
			lastValNorm = lastVal;
			if (hasLast) { lastVal = spec.map(lastValNorm) };
		};

	//	[\hasLast, hasLast, \curVal, curVal, \val, val].postln;

		if (
			(newValNorm.absdif(curValNorm) <= maxDiff)   // is Close Enough
									// or was the last value controller remembers.
			or: { hasLast and:
				{ curValNorm.absdif(lastValNorm) <= maxDiff }
			})
		{
			this.set(param, val);
			^true
		} {
			^false
		}
	}

		// val and lastVal are assumed to be mapped.
		// allows pausing when vol is 0.

	softVol_ { |val, within=0.025, pause=true, lastVal, spec|

		var curVolNorm, newVolNorm, hasLast, lastVolNorm;

		spec = (spec ? \amp).asSpec;
		hasLast = lastVal.notNil;

		curVolNorm = spec.unmap(this.vol);
		newVolNorm = spec.unmap(val);
		lastVolNorm = if (hasLast) { spec.unmap(lastVal) };

		if ( (curVolNorm.absdif(newVolNorm) <= within)
			or: { hasLast and: { curVolNorm.absdif(lastVolNorm) <= within } }
			)
		{

			this.vol_(val);

			if (pause) {
				if (val == 0) {
						// wait for vol to go down before pausing
					fork { 0.05.wait; this.pause }
				} {
					this.resume
				}
			};
			^true
		} {
			^false
		}
	}
}
