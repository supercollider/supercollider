
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

	//behave like a stream
	next { ^value }
	embedInStream { arg inval;
		^this.value.embedInStream(inval)
	}

	printOn { arg stream;
		stream << "`(" << value << ")";
	}
	storeOn { arg stream;
		stream << "`(" <<< value << ")";
	}
}

RefCopy : Ref
{
	next { ^value.copy }
}

