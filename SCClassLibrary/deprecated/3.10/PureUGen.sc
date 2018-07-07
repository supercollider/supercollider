// Something unusual: Deprecating an abstract class
// The fix is not to replace a method call, but to:
// 1. MyClass : UGen instead of MyClass : PureUGen
// 2. Add '*isPureUGen { ^true }' to the class definition.
// BOTH are needed.

PureUGen : UGen {
	*new1 { arg rate ... args;
		var superclass = this;
		while { superclass.superclass != PureUGen } {
			superclass = superclass.superclass;
		};
		"The abstract class PureUGen is deprecated. The UGen class '%'% needs to be updated to inherit from UGen, and implement *isPureUGen { ^true }. Please contact the UGen author."
		.format(
			this.name,
			if(superclass !== this) {
				", or its superclass '%',".format(superclass.name)
			} {
				""
			}
		).warn;
		^super.new1(rate, *args);
	}
	*multiNewList { arg args;
		var superclass = this;
		while { superclass.superclass != PureUGen } {
			superclass = superclass.superclass;
		};
		"The abstract class PureUGen is deprecated. The UGen class '%'% needs to be updated to inherit from UGen, and implement *isPureUGen { ^true }. Please contact the UGen author."
		.format(
			this.name,
			if(superclass !== this) {
				", or its superclass '%',".format(superclass.name)
			} {
				""
			}
		).warn;
		^super.multiNewList(args);
	}

	// An unwitting user might have written:
	// MyPseudoUGen : PureUGen { ... }
	// This is wrong -- pseudo-ugens should not inherit from UGen at all.
	// The above deprecation warnings will not catch that case.
	// So, at startup, check the class tree.
	*initClass {
		var children = this.subclasses;
		if(children.size > 0) {
			children = children.collect(_.name).sort;
			("The following classes inherit from PureUGen, which is now deprecated. Contact the UGen author(s) to update.\n" ++ children).warn;
		}
	}
}

// Multiple-inheritance problem:
// PureMultiOutUGen still needs to act like a MultiOutUGen

PureMultiOutUGen : MultiOutUGen {
	*new1 { arg rate ... args;
		var superclass = this;
		while { superclass.superclass != PureMultiOutUGen } {
			superclass = superclass.superclass;
		};
		"The abstract class PureMultiOutUGen is deprecated. The UGen class '%'% needs to be updated to inherit from MultiOutUGen, and implement *isPureUGen { ^true }. Please contact the UGen author."
		.format(
			this.name,
			if(superclass !== this) {
				", or its superclass '%',".format(superclass.name)
			} {
				""
			}
		).warn;
		^super.new1(rate, *args);
	}
	*multiNewList { arg args;
		var superclass = this;
		while { superclass.superclass != PureUGen } {
			superclass = superclass.superclass;
		};
		"The abstract class PureMultiOutUGen is deprecated. The UGen class '%'% needs to be updated to inherit from MultiOutUGen, and implement *isPureUGen { ^true }. Please contact the UGen author."
		.format(
			this.name,
			if(superclass !== this) {
				", or its superclass '%',".format(superclass.name)
			} {
				""
			}
		).warn;
		^super.multiNewList(args);
	}

	*initClass {
		var children = this.subclasses;
		if(children.size > 0) {
			children = children.collect(_.name).sort;
			("The following classes inherit from PureMultiOutUGen, which is now deprecated. Contact the UGen author(s) to update.\n" ++ children).warn;
		}
	}
}
