
+ Object {

	*test { arg resetFailures=true,report=true;
		var path,test;
		if(resetFailures,{ TestCase.resetFailures });
		path = TestCase.pathForClassTest(this);
		test = path.loadPath(false);
		if(test.notNil,{ 
			("Running test for " + this).inform;
			test.run(false,false);
			if(report,{ TestCase.report });
		},{
			// add this to failures
			("No TestCase found for " + this).inform;
		});
	}

}