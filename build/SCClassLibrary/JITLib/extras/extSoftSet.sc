	// Maybe rewrite as SoftSet(what, key, val, within, mapped, oldVal);

+ NodeProxy { 
				
	softSet { |param, val, within = 0.025, mapped=false, lastVal| 
		var spec, newNormVal, oldVal, oldNormVal, maxDiff;
		
		oldVal = this.nodeMap.get(param).value ?? { this.getDefaultVal(param) ? 0 };
			
		spec = param.asSpec; 
			
			// if no spec, guess whether step looks small:
		if (spec.isNil) { 
					// within is taken linear
			if ( 	(oldVal.absdif(val) <= within) 
			
					// or also try with an exponential range of ca 1 - 1000
				or: { max(oldVal / val, val / oldVal) <= (1000 ** within) })
				{ 
					this.set(param, val);
					^true
				} { 
					^false
				}
		};
			// simper if we have a valid spec:
		oldNormVal = spec.unmap( oldVal );
		maxDiff = max(within, spec.step);
		
		if (mapped) { 
			newNormVal = spec.unmap(val);
		//	lastVal = spec.unmap(lastVal); // lastVal is usually normed.
		} { 	
			newNormVal = val; 
			val = spec.map(val);
		};
		
		if (
			(newNormVal.absdif(oldNormVal) <= maxDiff)   // is Close Enough
									// or was the last value controller remembers.
			or: { lastVal.notNil and: 
				{ oldNormVal.absdif(lastVal) <= maxDiff } 
			}) 
		{ 
			this.set(param, val);
			^true
		} { 
			^false
		}
	}
	
	getDefaultVal { |key| 
		this.objects.do { |obj| 
			obj.controlNames.do { |ctlname|
				if (ctlname.name == key) { ^ctlname.defaultValue } 
			}
		};
		^nil
	}
		// val and lastVal are assumed to be mapped with amp.asSpec. 
		
	softVol_ { |val, within=0.025, pause=true, lastVal|
	
		var spec = \amp.asSpec;
		var oldPxNormVol = spec.unmap(this.vol);
		var newNormVol = spec.unmap(val); 
		var myOldNormVol = lastVal !? { spec.unmap(lastVal) };
		
	//	[\oldPxNormVol, oldPxNormVol, \newNormVol, newNormVol, \myOldNormVol, myOldNormVol].postln;
		
		if ( ((oldPxNormVol - newNormVol).abs < within)
			or: { myOldNormVol.notNil and: { oldPxNormVol.absdif(myOldNormVol) <= within } }
			)
		{
			this.vol_(val);
			if (pause) { 
				if (val == 0) { this.pause } { this.resume }
			};
			^true
		} { 
			^false
		}
	}
}
