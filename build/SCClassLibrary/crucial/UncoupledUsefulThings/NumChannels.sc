
NumChannels {

	*ar { arg input,numChannels=2,mixdown=true;
		
		if(input.size > 1,{// collection
		   ^input
			.clump(input.size / numChannels)
			.collect({arg chan,i;
				if(chan.size == 1,{
					chan.at(0)
				},{
					if(mixdown,{
						Mix.ar(chan)
					},{
						chan.at(0)
					})
				})
			})
		},{ // single ugen or single item collection
			if(input.isSequenceableCollection,{
				input = input.at(0);
			});
			
			if(numChannels == 1,{
				^input
			},{
				^Array.fill(numChannels,input)
			})
		})
	}
}

Mono {
	*new { arg input;		
		if(input.isArray,{
			^input.at(0)
		},{
			^input
		})
	}	
}


