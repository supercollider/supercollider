/*

Finalization is a way for the C primitives to release resources back to the system.

Rather than having a very complex system where any object can be finalized,
I have decided to centralize finalization in one class. This makes handling it in
the garbage collector very efficient.

Any class that needs finalizable data should create and point to an instance of
this class and put the data into it. This should be done from a primitive.
When the garbage collector collects this object it will call the C function you
stored in cFunction with this object as the parameter. You can also call the
finalize method to finalize on demand.

You should put your C function pointer into cFunction, and the finalizable object into 'object'.

*/

Finalizer {
	var cFunction, object; // no getters or setters!

	// no *new method! Create in a primitive.

	//finalize { _Finalize }
	notFinalized { ^cFunction.notNil }
	isFinalized { ^cFunction.isNil }
}
