// Fixed size ringbuffer
RingBuffer : SequenceableCollection
{
	var <array, <readPos, <writePos;

	*new { | size, collectionClass(Array) |
		^this.newCopyArgs(collectionClass.newClear(size), 0, 0)
	}

	// return maximum capacity.
	maxSize {
		^array.size
	}
	// return number of readable items.
	size {
		^this.readable
	}

	// return number of readble items.
	readable {
		^if (readPos <= writePos)
		{ writePos - readPos }
		{ array.size - readPos + writePos }
	}
	// return number of writable items.
	writable {
		^array.size - this.readable - 1
	}

	// add value and increase writePos.
	// do nothing if no items can be written.
	add { | value |
		if (this.writable > 0) {
			array.put(writePos, value);
			writePos = (writePos + 1) % array.size;
		}
	}
	// return next readable item and increase readPos.
	// return nil if no items can be read.
	pop {
		var result;
		if (this.readable > 0) {
			result = array.at(readPos);
			readPos = (readPos + 1) % array.size;
		};
		^result
	}
	// add value and increase writePos by overwriting oldest readable
	// item.
	overwrite { | value |
		var result;
		if (this.writable == 0) {
			result = this.pop;
		};
		this.add(value);
		^result
	}

	// iterate over the currently readable items.
	do { | function |
		var n = this.readable, i = 0;
		while { i < n } {
			function.value(array.wrapAt(readPos + i), i);
			i = i + 1;
		}
	}
}
