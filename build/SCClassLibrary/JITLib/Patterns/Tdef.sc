//task definition

Tdef : PauseStream {
	var <key, <isPlaying=false;
	
	*new { arg key, function;
		var t;
		key = key.asSymbol;
		
		t = this.at(key);
		
		if(t.isNil, { 
			t = super.new.function_(function);
			this.put(key, t);
		}, { 
			if(function.notNil, {
				t.function_(function);
			})
		 });
		 
		^t
	} 

	function_ { arg function;
		this.stream = Routine({ arg argList; 
							function.valueArray(argList);
					});
	}
	
	stream_ { arg argStream; 
		if(stream.notNil, {
			stream = argStream;
		});
		originalStream = argStream;
	}
	
	embed { arg inval;
		originalStream.embed(inval)
	}
	
	///////global access
	
	*at { arg key;
		^Library.at(this, key);
	}
	
	*put { arg key, item;
		item.prSetKey(key);
		Library.put(this, key, item);
	}
	
	prSetKey { arg argKey;
		key = argKey;
	}


}

