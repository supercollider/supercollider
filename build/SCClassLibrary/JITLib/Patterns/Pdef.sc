


Tdef : Task {
	
	*new { arg key, func, clock;
		var res;
		res = this.at(key);
		if(res.isNil, { 
				res = super.new(func, clock);
				this.put(key, res)
		}, {
			if(func.notNil, {
				res.stream = Routine(func)
			});
		})
		^res
	}
	*at { arg key;
		^Library.at(this, key)
	}
	*put { arg key, obj;
		Library.put(this, key, obj)
	}

}

Pdef : EventStream {

	*new { arg key, pat, event;
		var res, stream;
		res = this.at(key);
		stream = pat.asStream;
		if(res.isNil, { 
				res = super.new(stream, event);
				this.put(key, res)
		});
		if(stream.notNil, { res.stream = stream });
		if(event.notNil, { res.event = event });
		^res
	}
	*at { arg key;
		^Library.at(this, key)
	}
	*put { arg key, obj;
		Library.put(this, key, obj)
	}
	
	originalStream { ^originalStream }

}




