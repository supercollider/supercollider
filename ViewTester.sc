
/*
	abstract superclass for testing views on windows.
	ideally should leave window open on failure or error
*/

ViewTester : UnitTest {
	
	var w;
	var <>leaveWindowsOpen = false;
	
	setUp {
		w = Window.new;
		if(leaveWindowsOpen,{ w.front })
	}
	tearDown {
		if(leaveWindowsOpen.not,{
			if(w.notNil,{ w.close })
		})
	}
}

