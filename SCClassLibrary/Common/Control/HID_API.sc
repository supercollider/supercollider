HID_API {

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
                deviceList.put( i, HID_API_DeviceInfo.new( *it ) );
                // deviceList.put( i, HID_API_Device.new( i, it ) );
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
            newdev = HID_API_Device.new( newdevid );
            newdev.info = deviceList.at( id1 );
            newdev.getElements;
            openDevices.put( newdevid, newdev );
        }{
            newdevid = HID_API.prOpenDevice( id1,id2 );
            newdev = HID_API_Device.new( newdevid );
            newdev.getInfo;
            newdev.getElements;
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

   	*prGetNumberOfElements{ |joyid|
		_HID_API_GetNumberOfElements
		^this.primitiveFailed
	}

   	*prGetElementInfo{ |joyid,elid|
		_HID_API_GetElementInfo
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

HID_API_DeviceInfo{

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
        ^HID_API.prOpenDevice( vendorID, productID );
    }
}

HID_API_Device {

	var <id;
    var <>info;

    var <elements;

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
                elements.at( args[0] ).setValue( args[3], args[4] );
        };
		action.value( *args );
	}

    getElements{
        var numberOfElements = HID_API.prGetNumberOfElements( id );
        numberOfElements.do{ |i|
            var elInfo = this.getElementInfo( i );
            elements.put( i, HID_API_Element.new( *elInfo ) );
        }
    }

	getElementInfo{ |elid|
		^HID_API.prGetElementInfo( id, elid );
	}

	getInfo{
		var result = HID_API.prGetDeviceInfo( id );
		if ( result.isKindOf( Array ) ){
            info = HID_API_DeviceInfo.new( *result );
            // this.prettyPrint;
		}{
			"could not get info on hid device; maybe it is not open?".postln;
		}
	}

    // prettyPrint{
    //     "Device [%] : [%] has the following properties:\n".postf(id, name);
    //     [ ["number of axes",numAxes], ["number of buttons", numButtons ], [ "number of hats", numHats ], [ "number of balls", numBalls ], [ "unique identifier", guid ] ].do{ |it|
    //         "\t % : % \n".postf( it[0], it[1] );
    //     };
    // }

	close{
		^HID_API.prCloseDevice( id );
	}
}

HID_API_Element{

    classvar <>hutDirectory;

    var <index;
    var <io_type, <type;
    var <usage_page, <usage;
    var <logicalMin, <logicalMax;
    var <physicalMin, <physicalMax;
    var <unitExponent, <unit;
    var <reportSize, <reportID, <reportIndex;

    var <rawValue;
    var <logicalValue;

	var <value, <>action;

    var pageName, usageName, iotypeName, typeSpec;

    *initClass{
        hutDirectory = Platform.systemAppSupportDir +/+ "hid";
    }

	*new{ arg ...args;
        ^super.newCopyArgs( *args );
	}

    setValue{ |raw,logic|
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
        stream << $( << "hid element: " << index << ": " ;
        stream << "type and usage: ";
		[ io_type, type, usage_page, usage ].printItemsOn(stream);
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
            this.getUsageDescription;
        };
        ^pageName;
    }

    usageName{
        if ( usageName.isNil ){
            this.getUsageDescription;
        };
        ^usageName;
    }

    iotypeName{
        if ( iotypeName.isNil ){
            switch( io_type,
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

    getUsageDescription{
        var yamlfile;
        var huttable;
        switch( usage_page,
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
            13, { yamlfile = "hut_13_digitizer.yaml"; pageName = \Digitizer },
            16, { yamlfile = "hut_16_unicode.yaml"; pageName = \Unicode },
            20, { yamlfile = "hut_20_alphanumeric_display.yaml"; pageName = \AlphaNumericDisplay },
            64, { yamlfile = "hut_64_medical_instrument.yaml"; pageName = \MedicalInstrument },
            { nil; pageName = \undefined }
        );

        switch( yamlfile,
            \ordinal, { usageName = ("o"++usage).asSymbol; },
            \button, { usageName = ("b"++usage).asSymbol; },
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

    }

    readHUTFile{ |yamlfile|
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
    