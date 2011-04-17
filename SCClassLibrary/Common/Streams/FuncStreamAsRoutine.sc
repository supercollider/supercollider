FuncStreamAsRoutine : Routine {
	var <>nextFunc;
	var <>resetFunc;

	*new { arg nextFunc, resetFunc;
		^super.new({ arg inval;
			loop {
				inval = yield(thisThread.nextFunc.value(inval))
			}
		})
		.nextFunc_(nextFunc).resetFunc_(resetFunc)
	}

	reset { ^resetFunc.value }
}
