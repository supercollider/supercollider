//An Env describes a function of time that is sampled at the different time positions a Pattern
//assumes.  An Env may end at some other time position.  The variable endTime allows that time
//to be used to synchronize a subsequent Env in the stream.

VTRoutine : Routine {
	var <>endTime;
}
