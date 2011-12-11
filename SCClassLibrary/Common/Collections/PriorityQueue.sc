PriorityQueue {
	var array;

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

	removeValue {|value|
		var newObject = PriorityQueue();

		while {this.notEmpty} {
			var currentPriority = this.topPriority;
			var topObject = this.pop;
			if (topObject != value) {
				newObject.put(currentPriority, topObject)
			}
		};
		array = newObject.prInternalArray;
	}

	do {|func|
		if (array.size > 1) {
			forBy(1, array.size, 3) {|i|
				func.value(array[i+1],array[i+0])
			}
		}
	}

	// private
	prInternalArray {
		^array
	}
}
