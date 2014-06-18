+ LID {
    *buildDeviceTable{ |name|
        this.deprecated(thisMethod, LID.findMethod( \buildDeviceList ) );
		^LID.buildDeviceList( name );
	}

	*deviceTable{
        this.deprecated(thisMethod, LID.findMethod( \deviceList ) );
		^deviceList;
	}
}