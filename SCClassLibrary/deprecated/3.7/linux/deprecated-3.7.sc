+ LID {
	*buildDeviceTable{ |name|
		this.deprecated(thisMethod, LID.findMethod( \findAvailable ) );
		^LID.findAvailable( name );
	}

	*deviceTable{
		this.deprecated(thisMethod, LID.findMethod( \available ) );
		^available;
	}

	*buildDeviceList{ |name|
		this.deprecated(thisMethod, LID.findMethod( \findAvailable ) );
		^LID.findAvailable( name );
	}

	*deviceList{
		this.deprecated(thisMethod, LID.findMethod( \available ) );
		^available;
	}

	dumpEvents {
		this.deprecated(thisMethod, LID.findMethod( \debug_ ) );
		this.debug_( true );
	}

	*all {
		this.deprecated(thisMethod, LID.findMethod( \openDevices ) );
	 	^this.openDevices;
	}

	*eventLoopIsRunning{
		this.deprecated(thisMethod, LID.findMethod( \running ) );
	 	^this.running;
	}

}

+ LIDInfo {

	vendor{
		this.deprecated(thisMethod, LIDInfo.findMethod( \vendorID ) );
		^this.vendorID;
	}

	product{
		this.deprecated(thisMethod, LIDInfo.findMethod( \productID ) );
		^this.productID;
	}

}
