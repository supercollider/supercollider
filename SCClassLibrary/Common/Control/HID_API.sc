HID {

	classvar <running = false;

	classvar <deviceList;
	classvar <deviceNames;

    classvar <openDevices;

	classvar <action;
    classvar <deviceAction;

	classvar <>debug = false;

    classvar prAction;
    classvar prDeviceAction;

    *addRecvFunc{ |function|
        if ( prAction.isNil ){
            prAction = FunctionList.new;
        };
        prAction = prAction.addFunc( function );
    }

    *removeRecvFunc{ |function|
        prAction.removeFunc( function );
    }

    *addDevFunc{ |function|
        if ( prDeviceAction.isNil ){
            prDeviceAction = FunctionList.new;
        };
        prDeviceAction = prDeviceAction.addFunc( function );
    }

    *removeDevFunc{ |function|
        prDeviceAction.removeFunc( function );
    }

    *action_{ |function|
        if ( action.notNil ){
            this.removeRecvFunc( action );
        };
        action = function;
        this.addRecvFunc( function );
    }

    *deviceAction_{ |function|
        if ( deviceAction.notNil ){
            this.removeDevFunc( deviceAction );
        };
        deviceAction = function;
        this.addDevFunc( function );
    }

	*initClass{
		//Spec.initClass; ControlSpec.initClass;
		deviceList = IdentityDictionary.new;
        openDevices = IdentityDictionary.new;
	}

	*buildDeviceList{
		var devlist;
        // joyAxisSpec = Spec.add( \sdlJoyAxis, [ -32768, 32767, \linear, 1].asSpec );
		if ( running.not ){ this.start }; // start eventloop if not yet running
		devlist = this.prbuildDeviceList;
		devlist.postln;
		if ( devlist.isKindOf( Array ) ){
			deviceNames = devlist;
			devlist.do{ |it,i|
                deviceList.put( i, HIDDeviceInfo.new( *it ) );
			}{ // no devices found
				"no HID devices found".postln;
			}
		}
	}

    *open{ |id1,id2|
        var newdevid;
        var newdev;
        if ( id2.isNil ){
            newdevid = deviceList.at( id1 ).open;
            newdev = HIDDevice.new( newdevid );
            newdev.info = deviceList.at( id1 );
            newdev.getElements;
            newdev.getCollections;
            openDevices.put( newdevid, newdev );
        }{
            newdevid = HID.prOpenDevice( id1,id2 );
            newdev = HIDDevice.new( newdevid );
            newdev.getInfo;
            newdev.getElements;
            newdev.getCollections;
            openDevices.put( newdevid, newdev );
        }
        ^newdev;
    }

	*start{
		this.prStart;
		running = true;
		ShutDown.add( {this.stop} );
	}

	*stop{
        openDevices.do{ |it| it.close };
		this.prStop;
		running = false;
	}

  /// private

/// primitives called:
	*prStart{
		_HID_API_Start;
		^this.primitiveFailed
	}

	*prStop{
		_HID_API_Stop;
		^this.primitiveFailed
	}

	*prOpenDevice{ |joyid|
		_HID_API_OpenDevice
		^this.primitiveFailed
	}

	*prCloseDevice{ |joyid|
		_HID_API_CloseDevice
		^this.primitiveFailed
	}

	*prGetDeviceInfo{ |joyid|
		_HID_API_GetInfo
		^this.primitiveFailed
	}

    *prGetNumberOfCollections{ |joyid|
		_HID_API_GetNumberOfCollections
		^this.primitiveFailed
	}

   	*prGetCollectionInfo{ |joyid,colid|
		_HID_API_GetCollectionInfo
		^this.primitiveFailed
	}

   	*prGetNumberOfElements{ |joyid|
		_HID_API_GetNumberOfElements
		^this.primitiveFailed
	}

   	*prGetElementInfo{ |joyid,elid|
		_HID_API_GetElementInfo
		^this.primitiveFailed
	}

    *prSetElementOutput{ |joyid,elid,value|
		_HID_API_SetElementOutput
		^this.primitiveFailed
	}

	*prbuildDeviceList{
		_HID_API_BuildDeviceList
		^this.primitiveFailed
	}

// coming from the primitives:
/*
    *prHIDDeviceClosed{ |devid|
		prAction.value( \closed, devid );
		openDevices.at( devid ).closeAction.value;
		if ( debug ){
			[ devid, "closed" ].postln; // debugging
		};
        openDevices.removeAt( devid );
	}
*/

    *prHIDElementData { | devid, element, page, usage, value, mappedvalue |
        prAction.value( devid, element, page, usage, value, mappedvalue );
        openDevices.at( devid ).valueAction( element, page, usage, value, mappedvalue );
        // prAction.value( \hidelement, devid, element, page, usage, value, mappedvalue );
        // openDevices.at( devid ).valueAction( \element, element, page, usage, value, mappedvalue );
		if ( debug ){
			[ devid, "element data", devid, element, page, usage, value, mappedvalue ].postln; // debugging
		}
	}

    *prHIDDeviceData { | devid, numelements |
        prDeviceAction.value( devid, numelements );
        openDevices.at( devid ).devAction( numelements );
        // prAction.value( \hidupdate, devid, numelements );
        // openDevices.at( devid ).valueAction( \update, numelements );
		if ( debug ){
			[ devid, "device data", devid, numelements ].postln; // debugging
		}
	}

}

HIDDeviceInfo{

    var <vendorID, <productID;
    var <path;
    var <serialNumber;
    var <vendorName, <productName;
    var <releaseNumber;
    var <interfaceNumber;

    *new{ |...args|
        ^super.newCopyArgs( *args );
    }

    printOn { | stream |
		super.printOn(stream);
		stream << $( << vendorName << ", " << productName << ", ";
		[
			vendorID,
			productID
		].collect({ | x | "0x" ++ x.asHexString(4) }).printItemsOn(stream);
		stream << ", " << path << ", " << serialNumber << ", " << ", " << releaseNumber << ", " << interfaceNumber;
		stream.put($));
	}

    open{
        ^HID.prOpenDevice( vendorID, productID );
    }
}

HIDDevice {

	var <id;
    var <>info;

    var <elements;
    var <collections;

	var <>action;
    var <>deviceAction;
	var <>closeAction;
	var <>debug = false;

	*new{ |id|
		^super.new.init( id );
	}

	init{ |i|
		id = i;
        //info = HID_API_DeviceInfo.new( n );
        elements = IdentityDictionary.new;
        collections = IdentityDictionary.new;
        // elementDictionary = MultiLevelIdentityDictionary.new;
	}

    devAction{ arg ...args;
        if ( debug ){
            ( [ "device", id ] ++ args).postln;
		};
        deviceAction.value( *args );
    }

	valueAction{ arg ...args;
		if ( debug ){
            ([ "element", id ] ++ args).postln;
		};
        if ( elements.at( args[0] ).notNil ){
                elements.at( args[0] ).setValueFromInput( args[3], args[4] );
        };
		action.value( *args );
	}

    getCollections{
        var numberOfCollections = HID.prGetNumberOfCollections( id );
        numberOfCollections.do{ |i|
            var colInfo = this.getCollectionInfo( i );
            collections.put( i, HIDCollection.new( *colInfo ).device_( this ) );
        }
    }

	getCollectionInfo{ |colid|
		^HID.prGetCollectionInfo( id, colid );
	}

    getElements{
        var numberOfElements = HID.prGetNumberOfElements( id );
        numberOfElements.do{ |i|
            var elInfo = this.getElementInfo( i );
            elements.put( i, HIDElement.new( *elInfo ).device_( this ) );
        }
    }

	getElementInfo{ |elid|
		^HID.prGetElementInfo( id, elid );
	}

	getInfo{
		var result = HID.prGetDeviceInfo( id );
		if ( result.isKindOf( Array ) ){
            info = HIDDeviceInfo.new( *result );
            // this.prettyPrint;
		}{
			"could not get info on hid device; maybe it is not open?".postln;
		}
	}

    usagePage{ |col=0|
        collections.at( col ).usagePage;
    }

    usage{ |col=0|
        collections.at( col ).usage;
    }

    pageName{ |col=0|
        collections.at( col ).pageName;
    }

    usageName{ |col=0|
        collections.at( col ).usageName;
    }

    // prettyPrint{
    //     "Device [%] : [%] has the following properties:\n".postf(id, name);
    //     [ ["number of axes",numAxes], ["number of buttons", numButtons ], [ "number of hats", numHats ], [ "number of balls", numBalls ], [ "unique identifier", guid ] ].do{ |it|
    //         "\t % : % \n".postf( it[0], it[1] );
    //     };
    // }

	close{
		^HID.prCloseDevice( id );
	}
}

HIDCollection{
    var <index;
    var <type;
    var <usagePage, <usage;

    var <parent;
    var <numCollections;
    var <firstCollection;

    var <numElements;
    var <firstElement;

    var <>device;

    var pageName, usageName;

    *new{ arg ...args;
        ^super.newCopyArgs( *args );
	}

    pageName{
        if ( pageName.isNil ){
            #pageName, usageName = HIDUsage.getUsageDescription( usagePage, usage );
        };
        ^pageName;
    }

    usageName{
        if ( usageName.isNil ){
            #pageName, usageName = HIDUsage.getUsageDescription( usagePage, usage );
        };
        ^usageName;
    }

    printOn { | stream |
		super.printOn(stream);
        stream << $( << "hid collection: " << index << ": " ;
        stream << "type and usage: ";
		[ type, usagePage, usage ].printItemsOn(stream);
        stream << ": description: ";
        [ this.pageName, this.usageName ].printItemsOn(stream);
        stream << ": parent,: " << parent;
        stream << ": collections,: ";
        [ numCollections, firstCollection ].printItemsOn( stream );
        stream << ": elements,: ";
        [ numElements, firstElement ].printItemsOn( stream );
		stream.put($));
	}
}

HIDElement{

    var <index;
    var <ioType, <type;
    var <usagePage, <usage;
    var <logicalMin, <logicalMax;
    var <physicalMin, <physicalMax;
    var <unitExponent, <unit;
    var <reportSize, <reportID, <reportIndex;
    var <collection;

    var <rawValue;
    var <logicalValue;

	var <value, <>action;

    var <>device;

    var pageName, usageName, iotypeName, typeSpec;

	*new{ arg ...args;
        ^super.newCopyArgs( *args );
	}

    // use for sending output
    value_{ |val|
        //TODO: could remap this accordingly
        rawValue = val;
        value = val;
        HID.prSetElementOutput( device.id, index, rawValue );
    }

    // called from input report
    setValueFromInput{ |raw,logic|
        rawValue = raw;
        logicalValue = logic;
        // for now:
        value = logic;
        action.value( value );
    }

    // valueAction{ arg ...args;
    //     action.value( *args );
    //     value = args;
    // }

    printOn { | stream |
		super.printOn(stream);
        stream << $( << "hid element: " << index << ": collection " << collection << " : " ;
        stream << "type and usage: ";
		[ ioType, type, usagePage, usage ].printItemsOn(stream);
        stream << ": min and max: ";
        [ logicalMin, logicalMax, physicalMin, physicalMax ].printItemsOn(stream);
        stream << ": unit, exponent: ";
        [ unitExponent, unit ].printItemsOn(stream);
        stream << ": report: ";
        [ reportSize, reportID, reportIndex	].printItemsOn(stream);
        stream << ": description: ";
        [ this.pageName, this.usageName, this.iotypeName, this.typeSpec ].printItemsOn(stream);
		stream.put($));
	}

    pageName{
        if ( pageName.isNil ){
            #pageName, usageName = HIDUsage.getUsageDescription( usagePage, usage );
        };
        ^pageName;
    }

    usageName{
        if ( usageName.isNil ){
            #pageName, usageName = HIDUsage.getUsageDescription( usagePage, usage );
        };
        ^usageName;
    }

    iotypeName{
        if ( iotypeName.isNil ){
            switch( ioType,
                1, { iotypeName = \input },
                2, { iotypeName = \output },
                3, { iotypeName = \feature }
            );
        };
        ^iotypeName;
    }

    typeSpec{
        if ( typeSpec.isNil ){
            typeSpec = [
                ["Data","Constant"],
                ["Array","Variable"],
                ["Absolute","Relative"],
                ["NoWrap","Wrap"],
                ["Linear","NonLinear"],
                ["PreferredState","NoPreferred"],
                ["NoNullPosition", "NullState"],
                ["NonVolatile","Volatile"],
                ["BitField", "BufferedBytes"]
            ].collect{ |it,i|
                it.at( type.bitTest(i).binaryValue )
            };
        };
        ^typeSpec;
    }

}

HIDUsage {

    classvar <>hutDirectory;

    *initClass{
        hutDirectory = Platform.systemAppSupportDir +/+ "hid";
    }

    *getUsageDescription{ |usagePage,usage|
        var pageName, usageName;
        var yamlfile;
        var huttable;
        switch( usagePage,
            1, { yamlfile = "hut_1_generic_desktop.yaml"; pageName = \GenericDesktop },
            2, { yamlfile = "hut_2_simulation_controls.yaml"; pageName = \Simulation },
            3, { yamlfile = "hut_3_vr_controls.yaml"; pageName = \VR },
            4, { yamlfile = "hut_4_sport_controls.yaml"; pageName = \Sport },
            5, { yamlfile = "hut_5_game_controls.yaml"; pageName = \Game },
            6, { yamlfile = "hut_6_generic_device.yaml"; pageName = \GenericDevice },
            7, { yamlfile = "hut_7_keyboard_keypad.yaml"; pageName = \Keyboard },
            8, { yamlfile = "hut_8_ledpage.yaml"; pageName = \Led },
            9, { yamlfile = \button; pageName = \Button },
            10, { yamlfile = \ordinal; pageName = \Ordinal },
            11, { yamlfile = "hut_11_telephony.yaml"; pageName = \Telephony },
            12, { yamlfile = "hut_12_consumer.yaml"; pageName = \Consumer },
            13, { yamlfile = "hut_13_digitizers.yaml"; pageName = \Digitizer },
            16, { yamlfile = \unicode; pageName = \Unicode },
            20, { yamlfile = "hut_20_alphanumeric_display.yaml"; pageName = \AlphaNumericDisplay },
            64, { yamlfile = "hut_64_medical_instrument.yaml"; pageName = \MedicalInstrument },
            { nil; pageName = \undefined }
        );

        switch( yamlfile,
            \ordinal, { usageName = ("o"++usage).asSymbol; },
            \button, { usageName = ("b"++usage).asSymbol; },
            \unicode, { usageName = ("uc"++usage).asSymbol; },
            {
                if ( yamlfile.isNil ){
                    usageName = \undefined;
                }{
                    huttable = this.readHUTFile( yamlfile );
                    if ( huttable.at( usage ).notNil ){
                        usageName = huttable.at( usage ).at( \name );
                    }{
                        usageName = \undefined;
                    }
                }
            }
        );
        ^[ pageName, usageName ];
    }

    *readHUTFile{ |yamlfile|
        var huttable = (hutDirectory +/+ yamlfile).parseYAMLFile;
        var hutIdtable = IdentityDictionary.new;
        if ( huttable.isNil ){
            ^huttable;
        };
        huttable.keysValuesDo{ |k,v|
            var newkey = k.interpret;
            hutIdtable.put( newkey, IdentityDictionary.new );
            v.keysValuesDo{ |k2,v2|
                hutIdtable.at( newkey ).put(
                    k2.asSymbol, v2.asSymbol
                );
            };
        };
        ^hutIdtable;
    }

}