	// Maybe rewrite as SoftSet(what, key, val, within, mapped, oldVal);

+ NodeProxy { 
				
	softSet { |param, val, within = 0.025, mapped=false, lastVal| 
		var spec, newNormVal, oldNormVal;
		
		spec = param.asSpec;
		oldNormVal = spec.unmap(
			this.nodeMap.get(param).value 
			?? { this.getDefaultVal(param) ? 0 }
		);
		
		if (mapped) { 
			newNormVal = spec.unmap(val);
		//	lastVal = spec.unmap(lastVal); // lastVal is usually normed.
		} { 	
			newNormVal = val; 
			val = spec.map(val);
		};
	//	[ newNormVal, oldNormVal].postln; 
		
		if (
			(newNormVal.absdif(oldNormVal) <= max(within, spec.step))   // isCloseEnough
									// or was last value controller remembers.
			or: { lastVal.notNil and: { oldNormVal.absdif(lastVal) <= 
				max(within, spec.step) } }) 
		{ 
		//	"ok, close enough.".postln; 
			this.set(param, val);
			^true
		} { 
		//	"too far off.".postln;
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
