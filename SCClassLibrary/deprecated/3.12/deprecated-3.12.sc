+ ContiguousBlockAllocator {
	findPrevious { |address|
		this.deprecated(thisMethod, ContiguousBlockAllocator.findMethod(\findPrevious));
		^this.prFindPrevious(address)
	}

	findNext { |address|
		this.deprecated(thisMethod, ContiguousBlockAllocator.findMethod(\findNext));
		^this.prFindNext(address)
	}
}
