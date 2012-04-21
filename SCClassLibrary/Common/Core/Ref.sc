// a Ref is a handle to a value. you can use it to return results by reference
// example:
//		x = Ref.new(nil);
// 		z = obj.method(x); // method puts something in reference
//		x.value.doSomething; // retrieve value
//
// it is also used as a quoting device to insulate from multiChannelPerform in UGens
//
// A special syntax shortcut for Ref.new( expr ) is to use a backquote: `expr

Ref : AbstractFunction
{
	var <>value;
	*new { arg thing; ^super.new.value_(thing) }
	set { arg thing; value = thing }
	get { ^value }
	dereference { ^value }
	asRef { ^this }
	valueArray { ^value }
	valueEnvir { ^value }
	valueArrayEnvir { ^value }

	// behave like a stream
	next { ^value }
//	embedInStream { arg inval;
//		^this.value.embedInStream(inval)
//	}
	// prevent multichannel expansion in ugens
	asUGenInput { ^this }

	printOn { arg stream;
		stream << "`(" << value << ")";
	}
	storeOn { arg stream;
		stream << "`(" <<< value << ")";
	}
	at { | key | ^value.at(key) }
	put  { | key, val | value.put(key, val) }
	seq { | pat | value = pat.embedInStream(this) }
	asControlInput { ^value.asControlInput }

	// Some UGens take Buffer data which
	// the user might want to specify simply as `[0.9, 0.1, 0.3]
	asBufWithValues {
		^LocalBuf.newFrom(value);
	}

	// Allow to multichannel expand ugen specs, like those of Klank,
	// in the case of which two is the rank, but could be otherwise.
	multichannelExpandRef { |rank|
		var array, refarray;
		array = this.value.asArray;
		if(array.maxSizeAtDepth(rank) <= 1) { ^this }; // no need to expand
		refarray = array.flopDeep(rank).collect { |item| this.class.new(item) };
		^refarray.unbubble
	}
}

RefCopy : Ref
{
	next { ^value.copy }
}
