+ Object {

	asNSReturn{
		^this
	}

}

+ RawPointer{

	asNSReturn{
		^SCNSObject.newFromRawPointer(this)
	}
}
