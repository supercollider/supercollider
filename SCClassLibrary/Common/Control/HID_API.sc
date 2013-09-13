HID_API {

	classvar <running = false;

	classvar <deviceList;
	classvar <deviceNames;

    classvar <openDevices;

	classvar <action;

	classvar <>debug = false;

    classvar prAction; // this is where HIDFunc should hook into

    // classvar <joyAxisSpec;

    *addRecvFunc{ |function|
        prAction.addFunc( function );
    }

    *removeRecvFunc{ |function|
        prAction.removeFunc( function );
    }

    *action_{ |function|
        this.addRecvFunc( function );
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
	*prHIDDeviceClosed{ |devid|
		prAction.value( \closed, devid );
		openDevices.at( devid ).closeAction.value;
		if ( debug ){
			[ devid, "closed" ].postln; // debugging
		};
        openDevices.removeAt( devid );
	}

    *prHIDElementData { | devid, element, page, usage, value, mappedvalue |
		prAction.value( \hidelement, devid, element, page, usage, value, mappedvalue );
		openDevices.at( devid ).valueAction( \element, element, page, usage, value, mappedvalue );
		if ( debug ){
			[ devid, "element data", devid, element, page, usage, value, mappedvalue ].postln; // debugging
		}
	}

    *prHIDDeviceData { | devid, numelements |
		prAction.value( \hidupdate, devid, numelements );
		openDevices.at( devid ).valueAction( \update, numelements );
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

	valueAction{ arg ...args;
		if ( debug ){
			([ id ] ++ args).postln;
		};
        if ( args[0] == \element ){
            if ( elements.at( args[1] ).notNil ){
                elements.at( args[1] ).setValue( args[4], args[5] );
            };
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

	*new{ arg ...args;
        ^super.newCopyArgs( *args );
	}

    setValue{ |raw,logic|
        rawValue = raw;
        logicalValue = logic;
        // for now:
        value = logic;
    }

    // valueAction{ arg ...args;
    //     action.value( *args );
    //     value = args;
    // }

    printOn { | stream |
		super.printOn(stream);
        stream << $( << "hid element: " << index << ": " ;
		[
            "type and usage: ", io_type, type, usage_page, usage
		].printItemsOn(stream);
        [
            ": min and max: ", logicalMin, logicalMax, physicalMin, physicalMax, unitExponent, unit
		].printItemsOn(stream);
        [
            ": report: ", reportSize, reportID, reportIndex
		].printItemsOn(stream);
		stream.put($));
	}
}