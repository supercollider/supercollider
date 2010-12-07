PriorityQueue {
	var <array;

	put { arg time, item;
		_PriorityQueueAdd
		^this.primitiveFailed;
	}
	pop {
		_PriorityQueuePop
		^this.primitiveFailed;
	}
	topPriority {
		_PriorityQueueTop
		^this.primitiveFailed;
	}
	clear {
		_PriorityQueueClear
		^this.primitiveFailed;
	}
	postpone { arg time;
		_PriorityQueuePostpone
		^this.primitiveFailed;
	}
	isEmpty {
		_PriorityQueueEmpty
		^this.primitiveFailed;
	}
	notEmpty { ^this.isEmpty.not }
}