Pbgap : Pbindf {
	
	asStream {
		var streampairs, endval, eventStream;
		
		streampairs = patternpairs.copy;
		endval = streampairs.size - 1;
		forBy (1, endval, 2, { arg i;
			streampairs.put(i, streampairs.at(i).asStream);
		});
		eventStream = pattern.asStream;
		
		^FuncStream.new({ arg time;
			var event;
			var sawNil = false;
			event = eventStream.next(time);
			if (event.isNil, { ^nil });
			forBy (0, endval, 2, { arg i;
				var name, stream, streamout;
				name = streampairs.at(i);
				stream = streampairs.at(i+1);
				
				streamout = stream.next(time);
				
				if (streamout.isNil, {
					sawNil = true;
				},{
					if (name.isSequenceableCollection, {					
						streamout.do({ arg val, i;
							if(streamout !== N, {
								event.put(name.at(i), val);
							})
						});
					},{
						if(streamout !== N, {
							event.put(name, streamout);
						})
					});
				});
			});
			if (sawNil, { nil },{ 
				event 
			});
		},{
			streampairs = patternpairs.copy;
			endval = streampairs.size - 1;
			forBy (1, endval, 2, { arg i;
				streampairs.put(i, streampairs.at(i).asStream);
			});
		});
	}
}