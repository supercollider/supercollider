+ Dictionary {

	hatchAt { arg key=\argNames;
		var keys, res;
		keys = this.at(key);
		^if(keys.isNil or: { keys.isEmpty }, { [] }, { this.hatch(keys) })
	}
	
	hatch { arg keys;
			var res;
			res = Array.newClear(keys.size * 2);
			keys.do({ arg item, i;
					var val;
					val = this.at(item);
					res.put(2*i, item);
					res.put(2*i+1, val);
			});
			^res
	}
}