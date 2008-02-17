
TestPattern : UnitTest {

/*
	test_storeArgs {
		Pattern.allSubclasses.do({ |class|
			this.assert( class.findMethod('storeArgs').notNil
		
		})
	}
*/

/*
	var exceptions;
	exceptions = [PatternProxy,PbindProxy,Pbind,PFork,Pprob,Pflow,PlazyEnvirN,ListPattern,Pmono,Pfx,Pbindf,Pslide,Pwrand,Pseq,Pwalk,Pdfsm];

	Pattern.allSubclasses.reject(IsIn(exceptions)).do({ |class|
		var constructor,argNames,decoys,new;
		var storeArgs,asCompileString, recompile;
		// list a few exception classes here
	
		"".postln;
	
	//	num of args in constructor, 
		constructor = class.class.findMethod('new');
		if(constructor.isNil,{
			"no constructor".debug(class)
		},{
			argNames = constructor.argNames;
			decoys = argNames.copyRange(1,argNames.size-1).as(Array);
			decoys.debug(class);
			if(constructor.argNames.notNil,{
				//yep, we just fed it its own argNames
				new = class.performList('new',decoys);
			});
		
		//	do storeArgs
			storeArgs = new.storeArgs;
			if((storeArgs == decoys),{
				"TRUE!".debug;
			},{
				storeArgs.debug("failed storeArgs");
			})
		
	//	numbers should come back the same
	
	//	do asCompileString
	//	recompile
	//	
	//	should be the same
		})
		
	})
*/


}

