/* A thread stack local singleton class.
 * In unthreaded code it behaves like a singleton
 * In unnested threaded code it behaves like a thread local variable
 * In nested threaded code it looks up the nearest parent thread's local variable
 */

ThreadSingletonManager {
	var activePerThread;
	*new { ^super.newCopyArgs(IdentityDictionary()) }

	setActive {|o| ^activePerThread[thisThread] = o }
    removeActive {|instance| 
        activePerThread.keysValuesDo{|k, v|
            if(v === instance) {
                ^activePerThread[k] = nil
            }
        }
    }

    getActiveOwningThread {
		var t = thisThread;
		while {t.isNil.not} {
			if(activePerThread[t].isNil.not){
                ^t 
            };
			t = t.parent;
		};
        ^nil
    }

	getActive { ^activePerThread[this.getActiveOwningThread] }
}


