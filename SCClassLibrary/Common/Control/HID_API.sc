HID {

	classvar <running = false;

	classvar <available;

    classvar <openDevices;

	classvar <action;
    classvar <deviceAction;

	classvar <>debug = false;

    classvar prAction;
    classvar prDeviceAction;

    var <id;
    var <info;

    var <elements;
    var <collections;

	var <>action;
    var <>deviceAction;
	var <>closeAction;
	var <>debug = false;


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
		available = IdentityDictionary.new;
        openDevices = IdentityDictionary.new;
	}

	*findAvailable{
		var devlist;
        // joyAxisSpec = Spec.add( \sdlJoyAxis, [ -32768, 32767, \linear, 1].asSpec );
		if ( running.not ){ this.start }; // start eventloop if not yet running
		devlist = this.prbuildDeviceList;
		devlist.postln;
		if ( devlist.isKindOf( Array ) ){
			devlist.do{ |it,i|
                available.put( i, HIDInfo.new( *it ) );
			}{ // no devices found
				"no HID devices found".postln;
			}
		}
        ^available
	}

    *openAt{ |index|
        ^available.at( index ).open;
    }

    *open{ |vendorID, productID, path|
        var newdevid;
        var newdev;
        var thisone;
        if ( path.isNil ){
            thisone = HID.findBy( vendorID, productID ).asArray.first;
            path = thisone.path;
            "HID: path was not set specified yet, chosen the one with path: %\n".postf( path );
        };
        newdevid = HID.prOpenDevice( vendorID, productID, path );
        if ( newdevid == -1 ){
            "HID: Could not open device".error;
            ^nil;
        };
        newdev = HID.basicNew( newdevid );
        newdev.getInfo;
        ("HID: Opened device: %\n".postf( newdev.info ) );
        newdev.getElements;
        newdev.getCollections;
        openDevices.put( newdevid, newdev );
        ^newdev;
    }

    *openPath{ |path|
        var thisone = HID.findBy( path: path ).asArray.first;
        "HID: Opening device %\n".postf( thisone );
        ^thisone.open;
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

    *findBy { |vendorID, productID, path, serial, releaseNumber|
        if ( [vendorID, productID, path, serial, releaseNumber].every( _.isNil ) ){
			^nil;
		};
        ^HID.available.select { |info|
            vendorID.isNil or: { info.vendorID == vendorID }
            and: { productID.isNil or: { info.productID == productID } }
            and: { path.isNil or: { info.path == path } }
            and: { serial.isNil or: { info.serialNumber == serial } }
            and: { releaseNumber.isNil or: { info.releaseNumber == releaseNumber } }
        };
    }

  /// private

    *doPrAction{ | value, physValue, rawValue, arrayValue, usage, page, elid, devid |
        var thisdevice = openDevices.at( devid );
        // prAction.value( devid, thisdevice, elid, page, usage, value, mappedvalue );
        prAction.value( value, physValue, rawValue, arrayValue, usage, page, elid, devid, thisdevice );
    }

/// primitives called:
	*prStart{
		_HID_API_Start;
		^this.primitiveFailed
	}

	*prStop{
		_HID_API_Stop;
		^this.primitiveFailed
	}

	*prOpenDevice{ |vendorID, productID, path|
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

    *prSetElementRepeat{ |joyid,elid,value|
		_HID_API_SetElementRepeat
		^this.primitiveFailed
	}

	*prbuildDeviceList{
		_HID_API_BuildDeviceList
		^this.primitiveFailed
	}

// coming from the primitives:
    *prHIDDeviceClosed{ |devid|
		openDevices.at( devid ).closeAction.value;
		if ( debug ){
            "HID device % closed\n".postf( devid );
		};
        openDevices.removeAt( devid );
	}

    *prHIDElementData { | devid, elid, page, usage, rawValue, value, physValue, arrayValue |
        HID.doPrAction( value, physValue, rawValue, arrayValue, usage, page, elid, devid );
        openDevices.at( devid ).valueAction( value, physValue, rawValue,  arrayValue, usage, page, elid );
		if ( debug ){
            "HID Element Data:\n\tdevid: %, elid: %\n\telement: \t page: %\tusage: %\n\tarray value: %, \traw value: %,\tphysical value: %,\tvalue: %\n".postf( devid, elid, page, usage, arrayValue, rawValue, physValue, value );
            // [ devid, "element data", devid, element, page, usage, value, mappedvalue ].postln;
		}
	}

    *prHIDDeviceData { | devid, numelements |
        prDeviceAction.value( devid, numelements );
        openDevices.at( devid ).valueDeviceAction( numelements );
		if ( debug ){
			[ devid, "device data", devid, numelements ].postln; // debugging
		}
	}

// }
    // HIDDevice {
/*
	var <id;
    var <>info;

    var <elements;
    var <collections;

	var <>action;
    var <>deviceAction;
	var <>closeAction;
	var <>debug = false;
*/
    *new{ arg ...args;
        ^HID.open( *args );
    }

	*basicNew{ |id|
		^super.new.init( id );
	}

	init{ |i|
		id = i;
        elements = IdentityDictionary.new;
        collections = IdentityDictionary.new;
        // elementDictionary = MultiLevelIdentityDictionary.new;
	}

    valueDeviceAction{ arg ...args;
        if ( debug ){
            ( [ "device", id ] ++ args).postln;
		};
        deviceAction.value( *args );
        this.changed( \elementdata );
    }

	valueAction{ arg ...args;
        //     mappedvalue, value, usage, page, elid
		if ( debug ){
            ([ "element", id ] ++ args).postln;
		};
        if ( elements.at( args[6] ).notNil ){
            elements.at( args[6] ).setValueFromInput( args[0], args[1], args[2], args[3] );
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

    findElementWithUsage{ |elUsage, elUsagePage|
        ^elements.select{ |el|
            ( el.usage == elUsage ) and: ( el.usagePage == elUsagePage )
        }.asArray;
    }

	getElementInfo{ |elid|
		^HID.prGetElementInfo( id, elid );
	}

	getInfo{
		var result = HID.prGetDeviceInfo( id );
		if ( result.isKindOf( Array ) ){
            info = HIDInfo.new( *result );
            // this.prettyPrint;
		}{
			"could not get info on hid device; maybe it is not open?".postln;
		}
	}

    usagePage{ |collectionID=0|
        ^collections.at( collectionID ).usagePage;
    }

    usage{ |collectionID=0|
        ^collections.at( collectionID ).usage;
    }

    pageName{ |collectionID=0|
        ^collections.at( collectionID ).pageName;
    }

    usageName{ |collectionID=0|
        ^collections.at( collectionID ).usageName;
    }

    vendor{
        ^info.vendorID;
    }

    product{
        ^info.productID;
    }

    // prettyPrint{
    //     "Device [%] : [%] has the following properties:\n".postf(id, name);
    //     [ ["number of axes",numAxes], ["number of buttons", numButtons ], [ "number of hats", numHats ], [ "number of balls", numBalls ], [ "unique identifier", guid ] ].do{ |it|
    //         "\t % : % \n".postf( it[0], it[1] );
    //     };
    // }

	close{
		HID.prCloseDevice( id );
	}

    postCollections{
        this.collections.sortedKeysValuesDo{ |k,v| v.postCollection; };
    }

    postElements{
        this.elements.sortedKeysValuesDo{ |k,v| v.postElement; "".postln; };
    }

    postInputElements{
        this.elements.sortedKeysValuesDo{ |k,v| if( v.ioType == 1 ) { v.postElement; "".postln; } };
    }

    postOutputElements{
        this.elements.sortedKeysValuesDo{ |k,v| if( v.ioType == 2 ) { v.postElement; "".postln; } };
    }

    postFeatureElements{
        this.elements.sortedKeysValuesDo{ |k,v| if( v.ioType == 3 ) { v.postElement; "".postln; } };
    }

    *postAvailable{
        this.available.sortedKeysValuesDo{ |k,v| "%: ".postf( k ); v.postInfo; };
    }

    postInfo{
        info.postInfo;
    }
}

HIDInfo{

    var <vendorID, <productID;
    var <path;
    var <serialNumber;
    var <vendorName, <productName;
    var <releaseNumber;
    var <interfaceNumber;

    *new{ |...args|
        ^super.newCopyArgs( *args );
    }

    postInfo{
        "\tVendor name: \t%\n".postf( vendorName );
        "\tProduct name: \t%\n".postf( productName );
        "\tVendor and product ID: \t%, %\n".postf( vendorID, productID );
        "\tPath: \t%\n".postf( path );
        "\tSerial Number: \t%\n".postf( serialNumber );
        "\tReleasenumber and interfaceNumber: \t%,%\n".postf( releaseNumber, interfaceNumber );
    }

    printOn { | stream |
		super.printOn(stream);
		stream << $( << vendorName << ", " << productName << ", IDs:";
		[
			vendorID,
			productID
		]//.collect({ | x | "0x" ++ x.asHexString(4) })
        .printItemsOn(stream);
		stream << ", " << path << ", " << serialNumber << ", " << releaseNumber << ", " << interfaceNumber;
		stream.put($));
	}

    open{
        ^HID.open( vendorID, productID, path );
//        ^HID.open( vendorID, productID, serialNumber );
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

    postCollection{
        "HID Collection: %, type: %, usage page: %, usage index: %\n\tDescription: %, %\n\tParent collection: %, number of collections contained: %, first collection: %\n\tnumber of elements contained: %, first element %\n"
        .postf( index, type, usagePage, usage, this.pageName, this.usageName, parent, numCollections, firstCollection, numElements, firstElement );
    }

    printOn { | stream |
		super.printOn(stream);
        stream << $( << index << ": " ;
        stream << "type: " << type << "usage: " << usagePage << usage;
        // [ type, usagePage, usage ].printItemsOn(stream);
        // stream << ": description: ";
        // [ this.pageName, this.usageName ].printItemsOn(stream);
        // stream << ": parent,: " << parent;
        // stream << ": collections,: ";
        // [ numCollections, firstCollection ].printItemsOn( stream );
        // stream << ": elements,: ";
        // [ numElements, firstElement ].printItemsOn( stream );
		stream.put($));
	}
}

HIDElement{
//------- do not change the order of these variables, they correspond to the primitive data order: <-------
    var <index;
    var <ioType, <type;
    var <usagePage, <usage;
    var <usageMin, <usageMax;
    var <logicalMin, <logicalMax;
    var <physicalMin, <physicalMax;
    var <unitExponent, <unit;
    var <reportSize, <reportID, <reportIndex;
    var <rawValue;
    var <collection;
//-------> do not change the order of these variables -------

    var <logicalValue;
    var <physicalValue;
    var <arrayValue;

	var <value, <>action;

    var <>device;

    var <repeat = false;

    var pageName, usageName, iotypeName, typeSpec;

	*new{ arg ...args;
        ^super.newCopyArgs( *args ).mapValueFromRaw;
	}

    mapValueFromRaw{
        logicalValue = logicalMin + ( (logicalMax - logicalMin) * rawValue );
        // for now:
        value = logicalValue;
    }

    repeat_{ |rp|
        repeat = rp;
        HID.prSetElementRepeat( device.id, index, repeat.binaryValue );
    }

    // use for sending output
    value_{ |val|
        //TODO: could remap this accordingly
        rawValue = val;
        value = val;
        HID.prSetElementOutput( device.id, index, rawValue );
    }

    // called from input report
    setValueFromInput{ |logic,phys,raw, array|
        rawValue = raw;
        physicalValue = phys;
        logicalValue = logic;
        arrayValue = array;
        // for now:
        value = logic;
        action.value( value, this );
    }

    // valueAction{ arg ...args;
    //     action.value( *args );
    //     value = args;
    // }

    postElement{
            "HID Element: %, type: %, %, usage page: %, usage index: %\n\tDescription: %, %, %, \n\t% \n\tUsage range: [ %, % ]\n\tLogical range: [ %, % ]\n\tPhysical range: [ %, % ], Unit: %, Exponent: % \n\tReport ID: %, size %, index %\n"
        .postf( index, ioType, type, usagePage, usage, this.pageName, this.usageName, this.iotypeName, this.typeSpec,
            usageMin, usageMax,
            logicalMin, logicalMax, physicalMin, physicalMax, unit, unitExponent, reportID, reportSize, reportIndex);
    }

    printOn { | stream |
		super.printOn(stream);
        stream << $( << index << ": " ;
        stream << "type: " << type << ", usage: " << usagePage << usage;

        // stream << $( << "hid element: " << index << ": collection " << collection << " : " ;
        // stream << "type and usage: ";
        // [ ioType, type, usagePage, usage ].printItemsOn(stream);
        // stream << ": min and max: ";
        // [ logicalMin, logicalMax, physicalMin, physicalMax ].printItemsOn(stream);
        // stream << ": unit, exponent: ";
        // [ unitExponent, unit ].printItemsOn(stream);
        // stream << ": report: ";
        // [ reportSize, reportID, reportIndex	].printItemsOn(stream);
        // stream << ": description: ";
        // [ this.pageName, this.usageName, this.iotypeName, this.typeSpec ].printItemsOn(stream);
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
					if ( huttable.notNil ){
						if ( huttable.at( usage ).notNil ){
							usageName = huttable.at( usage ).at( \name );
						}{
							usageName = \undefined;
						}
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