+Group {
	releaseTail {
		if(tail.notNil, { tail.release });
	}
	releaseHead {
		if(head.notNil, { head.release });
	}
	freeTail {
		if(tail.notNil, { tail.free });
	}
	freeHead {
		if(head.notNil, { head.free });
	}

}