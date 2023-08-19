/* Used to create a single instance of some class that is accessible from anywhere.
 * This allows for different threads to each have their own instance, 
 *   or use their parent's.
 * Basic usage:
 *    1. make a classvar containing a ThreadSingletonManager called threadSingletonManager
 *      init in initClass - e.g. for class MyBuilder set `threadSingletonManager = ThreadSingletonManager(MyBuilder)`
 *    2. inherit from ThreadSingletonInstance or ThreadSingletonNestable depending on whether the instances can be nested
 *    3. call this.setActiveSingleton and this.removeActiveSingleton to push and pop the instance as the active
 *    4. to let other classes access the active instance, create a classmethod returning `threadSingletonManager.getActive` in MyBuilder (for example)
 *
 *
 * Basic example
 *
 * MyBuilder : ThreadSingletonInstance {
 *   classvar threadSingletonManager;
 *   *initClass { threadSingletonManager = ThreadSingletonManager(MyBuilder) }
 *   *getActiveSingleton { ^threadSingletonManager.getActive }
 *   with {|thingToDo|
 *      this.setActiveSingleton; // push this as active
 *      thingToDo.();
 *      this.removeActiveSingleton // remove this as active
 *   }
 * }
 */



ErrorThreadSingletonManager : Error {}

ThreadSingletonManager {
    var classname;
	var activePerThread;
	*new { |class| ^super.newCopyArgs(class.name, IdentityDictionary()) }

	setActive {|o| ^activePerThread[thisThread] = o }
    removeActive {|instance| 
        activePerThread.keysValuesDo{|k, v|
            if(v === instance) {
                ^activePerThread[k] = nil
            }
        };
		ErrorThreadSingletonManager("Could not find instance").throw;
    }

    getActiveOwningThread {
		var t = thisThread;
		while {t.isNil.not} {
			if(activePerThread[t].isNil.not){ ^t };
			t = t.parent;
		};
		ErrorThreadSingletonManager("Could not find thread").throw;
    }

	getActive { ^activePerThread[this.getActiveOwningThread] }
}

ThreadSingletonInstance {
	var threadSingletonManagerRef;
	setActiveSingleton { ^threadSingletonManagerRef.setActive(this) }
    removeActiveSingleton { ^threadSingletonManagerRef.removeActive(this) }
	getActiveSingleton { ^threadSingletonManagerRef.getActive }
}

ThreadSingletonNestable : ThreadSingletonInstance {
	var <maybeParent;
	doWith { |func|
		var old = this.getActiveSingleton;
        this.setActiveSingleton(this);
		func.();
		threadSingletonManagerRef.setActive(old);
		^this
	}
    // branch of family tree from root to this as tip
	getLineage { 
		var l = List();
		var c = this;
		while{c.isNil.not}{ l.append(c); c = c.maybeParent };
		^l.reverse
	}
}

