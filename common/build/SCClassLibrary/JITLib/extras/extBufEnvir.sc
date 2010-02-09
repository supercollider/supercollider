

+ ArrayedCollection {

	toBuffer { arg buffer;
//		buffer.server.makeBundle(nil, {
			buffer.numFrames_(this.size).numChannels_(1).alloc(buffer.setnMsg(0, this));
			// buffer.sendCollection(this);
			// doesn't work yet, because of the way makeBundle works.
//		});
	}

}

+ Nil {
	toBuffer { arg buffer;
		buffer.numFrames_(0).numChannels_(1).alloc
	}

}