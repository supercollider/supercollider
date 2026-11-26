Message {
	var <>receiver, <>selector, <>args;

	*new { |receiver, selector, args|
		^super.newCopyArgs(receiver, selector, args)
	}
	value { |... moreArgs|
		^receiver.performList(selector, args ++ moreArgs)
	}
	storeArgs { ^[receiver, selector, args] }
}

MethodQuote {
	var <>selector;

	*new { |selector|
		^super.newCopyArgs(selector)
	}
	value { |receiver ... args|
		^receiver.performList(selector, args)
	}
	storeArgs { ^[selector] }
}
