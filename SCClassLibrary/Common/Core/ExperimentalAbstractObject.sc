// AbstractObjectExperimental represents the minimum interface needed to get the interpreter to work (it will cause the IDE to post an error as it doesn't support printing).
// This makes it useful as a wrapper class and to implement some specific features.

// !!! --- PLEASE NEVER ADD A NORMAL PASCAL CASE METHOD TO ABSTRACT OBJECT --- !!!
// The whole point of this class is to be minimal.
// AbstractObjectExperimentals are not full Objects, allowing those who subclass it to create their own definition of what an object is and isn't.

AbstractObjectExperimental {

	/////// 1. Core and Interpreter functionality, do not override.

	*new { |maxSize = 0| _BasicNew; ^this.sc_abstract_object_primitive_failed }
	*newCopyArgs { | ... args, kwargs | _BasicNewCopyArgsToInstVars; ^this.sc_abstract_object_primitive_failed }

	// AbstractObjectExperimental support identity, this is required by the interpreter and cannot actually be overridden.
	// When you put AbstractObjectExperimentals into an identity dictionary the interpreter uses its own implementation, this should match the sclang implementation or things will get confusing.
	// Identity operators are also provided, this isn't strictly necessary, but we usually think of identity and these operators as a set.
	identityHash { _ObjectHash; ^this.sc_abstract_object_primitive_failed }
	=== { arg obj; _Identical; ^this.sc_abstract_object_primitive_failed }
	!== { arg obj;_NotIdentical; ^this.sc_abstract_object_primitive_failed }

	// Interpreter will engage in undefined behavior if this method does not throw.
	// It is not possible to implement a boolean abstract object as the compiler assumes its internal slot layout.
	mustBeBoolean {  MustBeBooleanError(nil, "AbstractObjectExperimental").throw }

	// It is not possible to implement a 'nil' abstract object due to the compiler inlining the definition of these when used inside 'if' control flow.
	// You cannot override these methods.
	isNil { ^false }
	notNil { ^true }
	? { |obj| ^this }
	?? { |obj| ^this }
	!? { |obj| ^obj.value(this) }



	/////// 2. Main interface, override these method.

	// Note: Interpreter will segfault if this isn't provided so a default is given.
	// You will need to override doesNotUnderstand to do anything useful with abstract objects,
	//   hence it throws a SubclassResponsibilityError rather than a DoesNotUnderstandError.
	// "AbstractObjectExperimental" is passed as a string here, rather than 'this', because 'this' may not support the 'toString' method, 	
	// which would cause the error handling to get stuck in a loop.
	
	doesNotUnderstand { |selector ...args, kwargs| 
		SubclassResponsibilityError("AbstractObjectExperimental", thisMethod, AbstractObjectExperimental).throw 
	}



	/////// 3. Helper methods and primitive implementations

	// These methods may be used *only* to implement other abstract objects, do not call them from outside the definition of an abstract object.
	// They are here so that other objects may use primitive implementations without making the primitives themselves publicly available.

	sc_abstract_object_primitive_failed { PrimitiveFailedError("AbstractObjectExperimental").throw }

	sc_abstract_object_hash { _ObjectHash; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_copy_shallow { _ObjectShallowCopy; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_copy_immutable { _ObjectCopyImmutable; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_copy_deep { _ObjectDeepCopy; ^this.sc_abstract_object_primitive_failed }

	// Yielding from a routine — must be implemented as primitives as special methods are provided.
	// Note that idle is not provided because it requires calling 'value' on the AbstractObjectExperimental'.
	sc_abstract_object_yield {	_RoutineYield; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_always_yield { _RoutineAlwaysYield; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_yield_and_reset { |reset = true| _RoutineYieldAndReset; ^this.sc_abstract_object_primitive_failed }

	// This is often necessary in abstract objects to determine if we have an abstract object, use isKindOf in normal use.
	sc_abstract_object_is_kind_of { |aClass| _ObjectIsKindOf; ^this.sc_abstract_object_primitive_failed }

	// Needed when we want to do the normal method calling in an AbstractObjectExperimental.
	// In many cases, the 'perform*' messages should defer to these as most of the time, you actually want to implement doesNotUnderstand.
	sc_abstract_object_perform_args { |selector, args, kwargs|	_ObjectPerformArgs;	^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_super_perform_args { |selector, args, kwargs| _ObjectSuperPerformArgs; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_perform_msg { |msg| _ObjectPerformMsg; ^this.sc_abstract_object_primitive_failed }
	sc_abstract_object_perform { |selector ... args, kwargs| _ObjectPerform; ^this.sc_abstract_object_primitive_failed }
	// Note that this is used to implement syntax, it is very strange to change this, please don't.
	sc_abstract_object_perform_list { | ...args, kwargs| _ObjectPerformList; ^this.sc_abstract_object_primitive_failed }
}
