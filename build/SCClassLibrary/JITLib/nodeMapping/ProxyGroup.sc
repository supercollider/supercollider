ProxyGroup : Group {
	var target, addAction, group;
	*new { arg target,addAction=\addToTail;
		^super.newCopyArgs(target.asTarget,addAction).initGroup;	}
	initGroup {
		group = Group.new(target, addAction);
	}
	group {
		^group ?? { this.initGroup; group }
	}
	
	
	free { group.free }
	isPlaying { ^group.isPlaying }
	doesNotUnderstand { arg selector ... args;
		^group.performList(selector, args);
	}
	
	
}