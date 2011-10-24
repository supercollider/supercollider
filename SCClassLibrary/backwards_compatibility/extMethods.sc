// The following interface in an optimized version of asControlInput that
// flattens arrayed values and marks the outermost array of a value with $[ and $]
// These Chars are turning into typetags ([ and ]) in the OSC message to mark that array
// Inner arrays are flattened (they are not meaningful in the server context)
// This makes it possible to write Synth("test", [0, [[100,200,300], [0.1,0.2,0.3], [10,20,30]] ])
// and have all the arguments be assigned to consecutive controls in the synth.

+Array {

	asOSCArgBundle {
		var array = Array(100);		// allocate a bunch of space
		this.do { | msg | array = array.add(msg.asOSCArgArray) };
		^array
	}

	asOSCArgArray {
		var array = Array(100);		// allocate a bunch of space
		this.do { | e | array = e.asOSCArgEmbeddedArray(array) };
		^array
	}

	asOSCArgEmbeddedArray { | array|
		array = array.add($[);
		this.do{ | e | array = e.asOSCArgEmbeddedArray(array) };
		array.add($]);
		^array;
	}

}

+Env {

	asControlInput { ^this.asArray }
	asOSCArgEmbeddedArray { | array| ^this.asArray.asOSCArgEmbeddedArray(array) }
}

+Object {
	asOSCArgEmbeddedArray { | array| ^array.add(this.asControlInput) }
	asOSCArgArray { ^this.asControlInput }
	asOSCArgBundle { ^this.asControlInput }
	asStringff { | size = 8 |
		var str = this.asString;
		str = str[0..size-1];
		str = str ++ String.fill(size - str.size, Char.space);
		^str;
	}

	postff { | size = 8 |
		this.asStringff(size).post
	}
}

+Nil {
	asOSCArgArray {}
}

+Ref {
	asOSCArgEmbeddedArray { | array| ^value.asOSCArgEmbeddedArray(array) }
}

+AbstractFunction {

	eq { arg function, adverb; ^this.composeBinaryOp('==', function, adverb) }
//
	ne { arg function, adverb; ^this.composeBinaryOp('!=', function, adverb) }

}
