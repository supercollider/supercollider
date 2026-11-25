FuncStreamAsRoutine : Routine {
	var <>nextFunc;
	var <>resetFunc;

	*new { |nextFunc, resetFunc|
		^super.new({ |inval|
			loop {
				inval = yield(thisThread.nextFunc.value(inval))
			}
		})
		.nextFunc_(nextFunc).resetFunc_(resetFunc)
	}

	reset { ^resetFunc.value }
}
