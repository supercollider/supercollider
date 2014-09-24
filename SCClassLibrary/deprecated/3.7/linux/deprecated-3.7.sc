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
		/*
		action = { | evtType, evtCode, value |
			[evtType.asHexString, evtCode.asHexString, value].postln;
		}
		*/
	}
}