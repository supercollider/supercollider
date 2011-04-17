Message {
	var <>receiver, <>selector, <>args;

	*new { arg receiver, selector, args;
		^super.newCopyArgs(receiver, selector, args);
	}
	value { arg ... moreArgs;
		^receiver.performList(selector, args ++ moreArgs);
	}
	storeArgs { ^[receiver, selector, args] }
}

MethodQuote {
	var <>selector;

	*new { arg selector;
		^super.newCopyArgs(selector);
	}
	value { arg receiver ... args;
		^receiver.performList(selector, args);
	}
	storeArgs { ^[selector] }
}
